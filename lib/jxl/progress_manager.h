#pragma once
#include <atomic>
#include <iostream>
#include <sstream>
#include <vector>

#include <chrono>
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
    inline std::string constructProgressString()
    {
      std::stringstream ss;
      if(currentFrame > 0)
      {
        ss<<"Frame"<<currentFrame<<"/"<<totalFrames<<" "<<frame_x<<"x"<<frame_y<<":";
      }
      for(const auto& st:steps)
      {
        ss<<st.name;
        if(st.printProg)
        {
          ss<<" "<<st.prog<<"/"<<st.totalProg;
          if(st.totalProg > 100)
          {
            ss<<" "<<(int)(((double)st.prog/(double)st.totalProg)*100.0)<<"%";
          }
        }
        if(st.printXY)
        {
          ss<<" "<<st.x<<"x"<<st.y;
        }
        ss<<st.extra<<"-";
      }
      return ss.str();
    }
    inline void print()
    {
      if(quiet)return;
      //std::cout<<"\033]0;"<<constructProgressString()<<"\007";
      //if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()- lastPrint).count() > 100)
      //{
        std::cout<<constructProgressString()<<std::endl;
        //lastPrint = std::chrono::high_resolution_clock::now();
      //}
    }
    inline void addStep(const step& st)
    {
      if(quiet)return;
      steps.push_back(st);
      print();
    }
    inline void popStep(const char* name = "")
    {
      if(quiet)return;
      if(std::string(name)!=steps.back().name)
      {
        std::cout<<std::endl<<"-----------_ERRORSTL-------- Popping:"<<steps.back().name<<" but tried to pop:"<<name<<std::endl<<std::endl;
      }

      steps.pop_back();
      if(true)print();
    }
    inline void advanceCurrentProg(uint32_t num = 1, bool printS = true)
    {
      if(quiet)return;
      steps.back().prog += num;
      if(printS)print();
      else if(steps.back().prog == steps.back().totalProg)print();
    }
    inline void addXY(uint32_t x, uint32_t y)
    {
      if(quiet)return;
      steps.back().x = x;
      steps.back().y = y;
      steps.back().printXY = true;
      print();
    }
    inline void addStringToStep(const char* string)
    {
      if(quiet)return;
      steps.back().extra = std::string(string);
      print();
    }
    inline void advanceTotalNumberOfFrames()
    {
      ++totalFrames;
      print();
    }
    inline void advanceFrameNumber()
    {
      ++currentFrame;
      print();
    }
    inline void addFrameInfoForProgress(uint32_t bits_per_sample,size_t x, size_t y)
    {
      frame_bitsPerSample = bits_per_sample;
      frame_x = x;
      frame_y = y;
      print();
    }
  }//namespace progress
}//namespace jpegxl