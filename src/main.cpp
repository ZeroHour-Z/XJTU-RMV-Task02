#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void processImage(const string& imagePath) {
    // 读取图像
    Mat image = imread(imagePath, IMREAD_COLOR);
    if (image.empty()) {
        cout << "Could not open or find the image" << endl;
        return;
    }

    // 转换为灰度图像
    Mat grayImage;
    cvtColor(image, grayImage, COLOR_BGR2GRAY);
    imwrite("../resources/gray_image.jpg", grayImage);
    // 转换为HSV图像
    Mat hsvImage;
    cvtColor(image, hsvImage, COLOR_BGR2HSV);
    imwrite("../resources/hsv_image.jpg", hsvImage);
    // 应用均值滤波
    Mat meanBlurredImage;
    blur(image, meanBlurredImage, Size(9, 9));
    imwrite("../resources/mean_blurred_image.jpg", meanBlurredImage);
    // 应用高斯滤波
    Mat gaussianBlurredImage;
    GaussianBlur(image, gaussianBlurredImage, Size(9, 9), 0);
    imwrite("../resources/gaussian_blurred_image.jpg", gaussianBlurredImage);
    // 创建红色区域的掩码, 提取红色区域
    Mat lowerRedMask, upperRedMask, redMask, redRegion;
    inRange(hsvImage, Scalar(0, 100, 100), Scalar(10, 255, 255), lowerRedMask);
    inRange(hsvImage, Scalar(160, 100, 100), Scalar(179, 255, 255), upperRedMask);
    // redMask = lowerRedMask | upperRedMask;
    add(lowerRedMask,upperRedMask,redMask);
    bitwise_and(image,image,redRegion,redMask);
    imwrite("../resources/red_region.jpg",redRegion);
    // 寻找红色区域的轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(redMask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    // 在原始图像上绘制轮廓
    Mat contourImage = image.clone();
    drawContours(contourImage, contours, -1, Scalar(0, 255, 0), 2);
    imwrite("../resources/red_contours.jpg", contourImage);
    // 在原始图像上绘制找到的边界框
    Mat boundingBoxImage = image.clone();
    for (size_t i = 0; i < contours.size(); i++) {
        Rect boundingBox = boundingRect(contours[i]);
        rectangle(boundingBoxImage, boundingBox, Scalar(0, 255, 0), 2);
    }
    imwrite("../resources/bounding_box_image.jpg", boundingBoxImage);
    // 计算轮廓面积
    for(size_t i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        if(area) cout << "Contour " << i << " area: " << area << endl;
    }
    cout << "Others are 0" << endl;
    // 已灰度化
    // 二值化
    Mat binaryImage;
    threshold(grayImage, binaryImage, 125, 255, THRESH_BINARY);
    imwrite("../resources/binary_image.jpg", binaryImage);
    // 膨胀
    Mat dilatedImage;
    dilate(binaryImage, dilatedImage, Mat(), Point(-1, -1), 2);
    imwrite("../resources/dilated_image.jpg", dilatedImage);
    // 腐蚀
    Mat erodedImage;
    erode(dilatedImage, erodedImage, Mat(), Point(-1, -1), 2);
    imwrite("../resources/eroded_image.jpg", erodedImage);
    // 漫水处理
    Mat floodFilledImage = erodedImage.clone();
    floodFill(floodFilledImage, Point(0, 0), Scalar(255));
    imwrite("../resources/floodfilled_image.jpg", floodFilledImage);
    // 反转漫水处理结果
    Mat floodFilledInvertedImage;
    bitwise_not(floodFilledImage, floodFilledInvertedImage);
    // 提取高亮区域
    Mat highLightedImage = (erodedImage | floodFilledImage);
    imwrite("../resources/high_lighted_image.jpg", highLightedImage);
    // 绘制任意圆形、方形和文字
    Mat drawingImage = image.clone();
    circle(drawingImage, Point(100, 100), 50, Scalar(255, 0, 0), 3);
    rectangle(drawingImage, Point(200, 200), Point(300, 300), Scalar(0, 255, 0), 3);
    putText(drawingImage, "Hello, OpenCV!", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
    imwrite("../resources/drawing_image.jpg", drawingImage);

    // 图像旋转35度
    Mat rotatedImage;
    Point2f center((image.cols - 1) / 2.0, (image.rows - 1) / 2.0);
    Mat rotationMatrix = getRotationMatrix2D(center, 35, 1.0);
    warpAffine(image, rotatedImage, rotationMatrix, image.size());
    imwrite("../resources/rotated_image.jpg", rotatedImage);
    // 图像裁剪为左上角1/4
    Rect roi(0, 0, image.cols / 2, image.rows / 2);
    Mat croppedImage = image(roi);
    imwrite("../resources/cropped_image.jpg", croppedImage);
}
int main(int argc, char** argv) {
    string imagePath = "/home/zerohour/opencv_project/resources/test_image.png";
    processImage(imagePath);
    return 0;
}
