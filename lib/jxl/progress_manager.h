#pragma once
#include <atomic>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <mutex>

#include <chrono>

//#define JXL_PROGRESS_USE_PRINT_THREAD
//#define JXL_PROGRESS_CHECK_MISMATCH
namespace jpegxl{
  namespace progress
  {
    struct step
    {
      step(){}
      step(std::string name, uint32_t totalProg=0, uint32_t prog=0, bool printProg=false)
      :name{name},totalProg{totalProg},prog{prog},printProg{printProg}
      {}
      std::string name;
      std::string extra;
      uint32_t totalProg{0};
      uint32_t prog{0};
      bool printProg{false};
      uint32_t x{0};
      uint32_t y{0};
      bool printXY{false};
    };
    inline bool quiet{false};
    inline std::vector<step> steps;
    inline std::atomic<uint32_t> currentFrame{0};
    inline std::atomic<uint32_t> totalFrames{0};
    inline std::chrono::time_point<std::chrono::high_resolution_clock> lastPrint;
    inline std::atomic<uint32_t> frame_bitsPerSample{0};
    inline std::atomic<uint32_t> frame_x{0};
    inline std::atomic<uint32_t> frame_y{0};
    inline std::atomic<bool> exitPrintThread = false;
    inline std::recursive_mutex progressMutex;
    inline std::string constructProgressString()
    {
      if(quiet)return("");
      std::stringstream ss;
      {
        std::scoped_lock lock(progressMutex);
        if(currentFrame > 0)
        {
          ss<<"Frame"<<currentFrame<<"/"<<totalFrames<<" "<<frame_x<<"x"<<frame_y<<":";
        }
        for(const auto& st:steps)
        {
          ss<<st.name;
          if(st.printProg == true)
          {
            ss<<" "<<st.prog<<"/"<<st.totalProg;
          }
          if(st.totalProg > 100)
          {
            ss<<" "<<(int)(((double)st.prog/(double)st.totalProg)*100.0)<<"%";
          }
          if(st.printXY)
          {
            ss<<" "<<st.x<<"x"<<st.y;
          }
          ss<<st.extra<<"-";
        }
      }
      return ss.str();
    }
    inline void addStep(const step& st)
    {
      if(quiet)return;
      std::scoped_lock lock(progressMutex);
      steps.push_back(st);
    }
    inline void popStep(const char* name = "")
    {
      if(quiet)return;
      std::scoped_lock lock(progressMutex);
      #ifdef JXL_PROGRESS_CHECK_MISMATCH
      if(std::string(name)!=steps.back().name)
      {
        std::cout<<std::endl<<"-----------_ERRORSTL-------- Popping:"<<steps.back().name<<" but tried to pop:"<<name<<std::endl<<std::endl;
      }
      #endif
      steps.pop_back();
    }
    inline void advanceCurrentProg(const char* name = "", uint32_t num = 1, bool printS = true)
    {
      if(quiet)return;
      std::scoped_lock lock(progressMutex);
      #ifdef JXL_PROGRESS_CHECK_MISMATCH
      if(std::string(name)!=steps.back().name)
      {
        std::cout<<std::endl<<"-----------_ERRORSTL-------- advancing:"<<steps.back().name<<" but tried to advance:"<<name<<std::endl<<std::endl;
      }
      #endif
      steps.back().prog += num;
    }
    inline void addXY(uint32_t x, uint32_t y)
    {
      if(quiet)return;
      std::scoped_lock lock(progressMutex);
      steps.back().x = x;
      steps.back().y = y;
      steps.back().printXY = true;
    }
    inline void addStringToStep(const char* string)
    {
      if(quiet)return;
      std::scoped_lock lock(progressMutex);
      steps.back().extra = std::string(string);
    }
    inline void advanceTotalNumberOfFrames()
    {
      ++totalFrames;
    }
    inline void advanceFrameNumber()
    {
      ++currentFrame;
    }
    inline void addFrameInfoForProgress(uint32_t bits_per_sample,size_t x, size_t y)
    {
      frame_bitsPerSample = bits_per_sample;
      frame_x = x;
      frame_y = y;
    }
    inline void printThread()
    {
      std::string lastProgressString, progressString;
      while(!exitPrintThread)
      {
        {
          progressString = constructProgressString();
          if(lastProgressString != progressString)
          {
            std::cout<<progressString<<std::endl;
            lastProgressString = progressString;
          }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(333));
      }
    }
    inline void startThread()
    {
      std::thread(printThread).detach();
    }
    inline void exitThread()
    {
      exitPrintThread = true;
    }
  }//namespace progress
}//namespace jpegxl