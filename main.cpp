#include "LD_Smarpod.h"
// TODO: Find SmarFunctions from AliceCalibrate and convert to using this instead.

#include <fstream>
#include <iostream>
#include <SmarPod.h>

int main(){
//    char outBuffer[4096];
//    unsigned int bufferSize = sizeof(outBuffer);
//    Smarpod_Status result = Smarpod_FindSystems("", outBuffer, &bufferSize);
//    if(result == SMARPOD_OK){
//        std::cout << "Found: " << outBuffer << std::endl;
//    }
//    else{
//        std::cout << "Find systems says: " << result << std::endl;
//    }
//
    LD_Smarpod mySmarpod;

    mySmarpod.Move_Absolute({0.001, 0, 0, 0, 0, 0});
    mySmarpod.Move_Absolute({0.001, 0.001, 0, 0, 0, 0});
    mySmarpod.Move_Absolute({0.0, 0.001, 0, 0, 0, 0});
    mySmarpod.Move_Absolute({0, 0, 0, 0, 0, 0});

//    std::vector<Smarpod_Pose> poses = SpiralScan(0,0,0,10,10,0.5);
//    Poses_To_File(poses, "poses.csv");

    return 0;
}
