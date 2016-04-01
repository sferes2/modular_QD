#ifndef ROBOT_ARM_HORI_HPP
#define ROBOT_ARM_HORI_HPP


#include <Eigen/Core>

namespace robot
{
    class Arm
    {
    public:
        
      static Eigen::Vector3d forward_model(Eigen::VectorXd a);
      static double max_length(){return p1_height+p2_height+p3_height+p4_height+p5_height+p6_height+p7_height+p8_height;}
    protected:

    
        static constexpr  double p1_height = 0.0875;
        static constexpr  double p2_height = 0.078;
        static constexpr  double p3_height = 0.072;
        static constexpr  double p4_height = 0.072;
        static constexpr  double p5_height = 0.071;
        static constexpr  double p6_height = 0.068;
        static constexpr  double p7_height = 0.068;
        static constexpr  double p8_height = 0.065;


    };
}

#endif
