//g++ -std=c++11 IR_sample.cpp -lrealsense2 -lopencv_core -lopencv_highgui -o ir `pkg-config --cflags --libs opencv`
// include the librealsense C++ header file
#include <librealsense2/rs.hpp>

// include OpenCV header file
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

int D435_IR()
{
int width = 1280;
int height = 720;
int fps = 30;
rs2::config config;
config.enable_stream(RS2_STREAM_INFRARED, 1, width, height, RS2_FORMAT_Y8, fps);
config.enable_stream(RS2_STREAM_INFRARED, 2, width, height, RS2_FORMAT_Y8, fps);

// start pipeline
rs2::pipeline pipeline;
rs2::pipeline_profile pipeline_profile = pipeline.start(config);


while (1) // Application still alive?
{
// wait for frames and get frameset
rs2::frameset frameset = pipeline.wait_for_frames();


// get single infrared frame from frameset
//rs2::video_frame ir_frame = frameset.get_infrared_frame();


// get left and right infrared frames from frameset
rs2::video_frame ir_frame_left = frameset.get_infrared_frame(1);
rs2::video_frame ir_frame_right = frameset.get_infrared_frame(2);


cv::Mat dMat_left = cv::Mat(cv::Size(width, height), CV_8UC1, (void*)ir_frame_left.get_data());
cv::Mat dMat_right = cv::Mat(cv::Size(width, height), CV_8UC1, (void*)ir_frame_right.get_data());


cv::imshow("img_l", dMat_left);
cv::imshow("img_r", dMat_right);
char c = cv::waitKey(1);
if (c == 's')
{

}
else if (c == 'q')
break;
}


return EXIT_SUCCESS;

}


int main(int argc, char* argv[])
{
    // #define DEBUG
    // #ifdef DEBUG
    // D435_IR();
    // return 0;
    // #endif
    string irFile = "ir.mp4";
    string depthFile = "depth.mp4";
    int timeout = 10;
    if (argc ==3 ){
        irFile = argv[1];
        timeout = stoi(argv[2]);
    }
    cout <<"Record IR frame to file " <<irFile <<" for " <<timeout << " seconds" <<endl;
    cout <<"To customize output file and duration, use ./ir <output file> <duration>" <<endl;
    //Contruct a pipeline which abstracts the device
    rs2::pipeline pipe;

    //Create a configuration for configuring the pipeline with a non default profile
    rs2::config cfg;

    //Add desired streams to configuration
    cfg.enable_stream(RS2_STREAM_INFRARED, 640, 480, RS2_FORMAT_Y8, 30);
    cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 30);

    //Instruct pipeline to start streaming with the requested configuration
    pipe.start(cfg);

    // Camera warmup - dropping several first frames to let auto-exposure stabilize
    rs2::frameset frames;
    for(int i = 0; i < 30; i++)
    {
        //Wait for all configured streams to produce a frame
        frames = pipe.wait_for_frames();
    }

    //OpenCV
    
    VideoWriter irVideo;
    VideoWriter depthVideo;
    irVideo.open(irFile, CV_FOURCC('a','v','c','1'), 30.0, Size(640,480),false);
    depthVideo.open(depthFile, CV_FOURCC('a','v','c','1'), 30.0, Size(640,480),false);
    if (!irVideo.isOpened()){
        cout << "Fail to open" <<endl;
        return -1;
    }
    int count = 0;
    const auto window_name = "Display Image";
    namedWindow(window_name, WINDOW_AUTOSIZE);

    // namedWindow("Display Image", WINDOW_AUTOSIZE );
    while(true) {
        rs2::frameset frames = pipe.wait_for_frames(); // Wait for next set of frames from the camera

        //Get each frame
        rs2::frame ir_frame = frames.get_infrared_frame();
        rs2::frame depth_frame = frames.get_depth_frame();

        // Creating OpenCV matrix from IR image
        // cv::Mat ir = cv::Mat(cv::Size(640, 480), CV_8UC1, (void*)ir_frame.get_data(), Mat::AUTO_STEP);
        // Mat image(Size(w, h), CV_8UC3, (void*)depth.get_data(), Mat::AUTO_STEP);
        Mat ir(Size(640, 480), CV_8UC1, (void*)ir_frame.get_data(), Mat::AUTO_STEP);
        // cv::Mat depth = cv::Mat(cv::Size(640, 480), CV_16SC1, (void*)depth_frame.get_data(), Mat::AUTO_STEP);
        // cv::Mat DD;
        // depth.convertTo(DD, CV_8U, 1, 0);
        

        imshow(window_name, ir);
        // cv::imshow("Depth",depth);
        waitKey(1);
        irVideo << ir;
        // depthVideo << DD;
        ++count;
        // string outimg;
        // outimg = "ir_"; 
        // outimg += to_string(count);
        // outimg += ".png";
        // imwrite(outimg, ir);
        
        if(count == timeout*30) break;
    }
    
    // Apply Histogram Equalization
    // equalizeHist( ir, ir );
    // applyColorMap(ir, ir, COLORMAP_JET);

    // Display the image in GUI
    
    // imwrite("ir.png", ir);

    // waitKey(0);

    return 0;
}
