
#include "arm_hori.hpp"



namespace robot{


  Eigen::Vector3d Arm::forward_model(Eigen::VectorXd a){
      Eigen::VectorXd _l_arm=Eigen::VectorXd(9);
	    _l_arm(0)=0;
	    _l_arm(1)=p1_height;
	    _l_arm(2)=p2_height;
	    _l_arm(3)=p3_height;
	    _l_arm(4)=p4_height;
	    _l_arm(5)=p5_height;
	    _l_arm(6)=p6_height;
	    _l_arm(7)=p7_height;
	    _l_arm(8)=p8_height;

    a=-a;  // change the angles' signe to match the servos' orientation. 

    //    std::vector<Eigen::Matrix4d> mats;
    Eigen::Matrix4d mat=Eigen::Matrix4d::Identity(4,4);
    for(size_t i=0;i<a.size();i++){
      Eigen::Matrix4d submat;
      submat<<cos(a(i)), -sin(a(i)), 0, _l_arm(i), sin(a(i)), cos(a(i)), 0 , 0, 0, 0, 1, 0, 0, 0, 0, 1;
      //mats.push_back(submat);
      mat=mat*submat;
    }
    Eigen::Matrix4d submat;
    submat<<1, 0, 0, _l_arm(a.size()), 0, 1, 0 , 0, 0, 0, 1, 0, 0, 0, 0, 1; // IF ROBOT NOT DIVIDE BY 2 else: // divide by 2 the last segment to get is CoM position. 
    //mats.push_back(submat);
    mat=mat*submat;
    Eigen::VectorXd v=mat*Eigen::Vector4d(0,0,0,1);

    return v.head(3);

  }


}
