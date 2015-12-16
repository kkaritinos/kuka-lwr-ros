#include "kuka_fri_bridge/kuka_fri_interface.h"
#include "kuka_fri_bridge/utilities.h"

namespace kfb{

Fri_interface::Fri_interface(ros::NodeHandle& nh){

    fri_pub             = nh.advertise<kuka_fri_bridge::FRI>("FRI_data", 10);
    joint_state_pub     = nh.advertise<sensor_msgs::JointState>("joint_states",10);

    fri_data.stiffness.resize(NB_JOINTS);
    fri_data.damping.resize(NB_JOINTS);
    fri_data.effort.resize(NB_JOINTS);
    fri_data.position.resize(NB_JOINTS);

    joint_states_msg.name.resize(NB_JOINTS);
    joint_states_msg.position.resize(NB_JOINTS);
    joint_states_msg.velocity.resize(NB_JOINTS);
    joint_states_msg.effort.resize(NB_JOINTS);
    for(std::size_t i = 0; i < NB_JOINTS;i++){
        joint_states_msg.name[i] = "lwr_" + boost::lexical_cast<std::string>(i) + "_joint";
    }


    if(!nh.getParam("/fri_drivers",fri_drivers_path))
    {
        ROS_ERROR("failed to find fri_drivers rosparameter");
    }else{
        std::cout<< "fri_drivers: " << fri_drivers_path << std::endl;
    }



}

void Fri_interface::start_fri(){
    mFRI = boost::shared_ptr<FastResearchInterface>( new FastResearchInterface(fri_drivers_path.c_str()) );
}

void Fri_interface::publish(const kfb::LWRRobot_FRI& kuka_robot){


    fri_data.FRI_QUALITY               = mFRI->GetCommunicationTimingQuality();
    fri_data.FRI_STATE                 = mFRI->GetFRIMode();
    fri_data.FRI_CTRL                  = FastFRI_CTRL2FRI_CTRL(mFRI->GetCurrentControlScheme());
    fri_data.DoesAnyDriveSignalAnError = mFRI->DoesAnyDriveSignalAnError();
    fri_data.IsRobotArmPowerOn         = mFRI->IsRobotArmPowerOn();
    fri_data.position                  = kuka_robot.joint_position_;
    fri_data.effort                    = kuka_robot.joint_effort_;
    fri_data.damping                   = kuka_robot.joint_damping_;
    fri_data.stiffness                 = kuka_robot.joint_stiffness_;

    joint_states_msg.header.stamp      = ros::Time::now();
    joint_states_msg.position          = kuka_robot.joint_position_;
    joint_states_msg.velocity          = kuka_robot.joint_velocity_;
    joint_states_msg.effort            = kuka_robot.joint_effort_;

    fri_pub.publish(fri_data);
    joint_state_pub.publish(joint_states_msg);
}

}
