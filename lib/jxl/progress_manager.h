#pragma once
#include <atomic>
#include <iostream>
#include <sstream>
#include <vector>

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
      uint32_t totalProg{0};
      uint32_t prog{0};
      bool printProg{false};
      uint32_t x{0};
      uint32_t y{0};
      bool printXY{false};
    };
    inline std::vector<step> steps;
    inline std::atomic<uint32_t> currentFrame{0};
    inline std::atomic<uint32_t> totalFrames{0};
    inline std::string constructProgressString()
    {
      std::stringstream ss;
      for(const auto& st:steps)
      {
        ss<<st.name;
        if(st.printProg)
        {
          ss<<" "<<st.prog<<"/"<<st.totalProg;
        }
        if(st.printXY)
        {
          ss<<" "<<st.x<<"x"<<st.y;
        }
        ss<<"->";
      }
      return ss.str();
    }
    inline void print()
    {
      //std::cout<<"\033]0;"<<constructProgressString()<<"\007";
      std::cout<<constructProgressString()<<std::endl;
    }
    inline void addStep(const step& st)
    {
      steps.push_back(st);
      print();
    }
    inline void popStep(bool printNow = true)
    {
      steps.pop_back();
      if(printNow)print();
    }
    inline void advanceCurrentProg(uint32_t num = 1)
    {
      steps.back().prog += num;
      print();
    }
    inline void addXY(uint32_t x, uint32_t y)
    {
      steps.back().x = x;
      steps.back().y = y;
      steps.back().printXY = true;
      print();
    }
  }//namespace progress
}//namespace jpegxl

/*
  jpegxl::progress::popStep(false);
  jpegxl::progress::addStep(jpegxl::progress::step(codecString));
*/