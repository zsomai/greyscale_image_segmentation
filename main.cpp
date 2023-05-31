#include <cstdlib>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "bin_tree.h"

using namespace cv;

struct Highlight_T {
    int value;
    Vec3b color;
    Highlight_T( int v, Vec3b c){
        value = v;
        color = c;
    }
};

void showHistogram(const char* name, int* hist, const int  hist_cols, const int hist_height) {
    Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255));

    int max_hist = 0;
    for (int i = 0; i<hist_cols; i++)
        if (hist[i] > max_hist)
            max_hist = hist[i];
    double scale = 1.0;
    scale = (double)hist_height / max_hist;
    int baseline = hist_height - 1;
    for (int x = 0; x < hist_cols; x++) {
        Point p1 = Point(x, baseline);
        Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
        Vec3b color(255, 0, 255);
        line(imgHist, p1, p2, color);   // histogram bins
        // colored in magenta
    }
    imshow(name, imgHist);
}

void showHistogram(const char* name, int* hist, const int  hist_cols, const int hist_height, std::vector<Highlight_T> highlights) {
    Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255));

    int max_hist = 0;
    for (int i = 0; i<hist_cols; i++)
        if (hist[i] > max_hist)
            max_hist = hist[i];
    double scale = 1.0;
    scale = (double)hist_height / max_hist;
    int baseline = hist_height - 1;
    for (int x = 0; x < hist_cols; x++) {
        Point p1 = Point(x, baseline);
        Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
        Vec3b color(255, 0, 255);
        for (Highlight_T highlight : highlights){
            if (highlight.value == x){
                color = highlight.color;
                break;
            }
        }
        line(imgHist, p1, p2, color);   // histogram bins
        // colored in magenta
    }
    imshow(name, imgHist);
}

int* calculate_histogram(Mat src){
    int* hist = (int*)calloc(256, sizeof(int));
    for(int i = 0; i < src.rows; i++)
        for(int j = 0; j < src.cols; j++)
            hist[src.at<uchar>(i,j)]++;

    return hist;
}

bin_tree* binarization_recursive_helper(int* histo, int colorsToDivide, int offset = 0, int len = 256){
    ///FIND THRESHOLD
    int lower_limit, upper_limit;
    int bestT = 0;
    double bestTF = 0;
    for (int i = offset; i < len; i++){
        long long s1 = 0, s2 = 0, s3 = 0, s4 = 0;
        for(int j = offset; j < i; j++){
            s1 += j*histo[j];
            s2 += histo[j];
        }
        for(int j = i; j < len; j++){
            s3 += j*histo[j];
            s4 += histo[j];
        }
        double f = pow(s1, 2)/s2 + pow(s3,2)/s4;
        if(bestTF < f) {bestTF = f; bestT = i;}
    }
    bin_tree* head = new bin_tree(bestT);
    if(colorsToDivide > 2) {
        head->setLeft(binarization_recursive_helper( histo, colorsToDivide / 2,offset, bestT));
        head->setRight(binarization_recursive_helper(histo, colorsToDivide / 2,bestT, len));

        lower_limit = head->getLeft() -> getRightest() -> getVal();
        upper_limit = head->getRight()-> getLeftest()  -> getVal();
    } if (colorsToDivide == 2){
        lower_limit = offset;
        upper_limit = len;

    }
    ///FIND COLORS
    int s1 = 0, s2 = 0, s3 = 0, s4 = 0;


    for (int i = lower_limit; i < bestT; i++){
        s1 += i*histo[i];
        s2 += histo[i];
    }
    int a = s1 / s2;
    std::vector<int> colors{a};

    if(upper_limit == 256){
        for (int i = bestT; i < upper_limit; i++) {
            s3 += i * histo[i];
            s4 += histo[i];
        }
        int b = s3 / s4;
        colors.push_back(b);
    }

    head->setColors(colors);
    return head;
}

///Image binarization with greyscale approximation
bin_tree* binarization(Mat src){
    int* histo = calculate_histogram(src);
    bin_tree* head;
    printf("Number of intensities (power of 2): ");
    int n;
    scanf("%d", &n);
    head = binarization_recursive_helper(histo, n);
    free(histo);
    return head;
}

///Image binarization
void greyScaleToBlackAndWhite(Mat img,Mat& output,int th){
    for (int i = 0; i < img.rows; i++)
        for (int j = 0; j < img.cols; j++) {
            if(img.at<uchar>(i,j) > th) {
                output.at<uchar>(i,j) = 255;
            } else {
                output.at<uchar>(i,j) = 0;
            }

        }
}

float calculate_otsu_threshold(int* histo, int t){
    ///Calculate weights
    int num_low = 0, num_high = 0;
    float mean_low = 0, mean_high = 0;
    for(int i = 0 ; i < t; i++){
        num_low += histo[i];
        mean_low += histo[i] * i;
    }
    for(int i = t; i < 256; i++){
        num_high += histo[i];
        mean_high += histo[i] * i;
    }
    float w1 = (float)num_low / (num_low + num_high);
    float w2 = 1 - w1;

    if (w1 == 0 || w2 == 0) return INFINITY;

    mean_high /= num_high;
    mean_low /= num_low;
    float var_low = 0, var_high = 0;
    for(int i = 0; i < t; i++)
        var_low += pow(i - mean_low, 2) * histo[i];

    for(int i = t; i < 256; i++)
        var_high += pow(i - mean_high, 2) * histo[i];
    var_low /= num_low;
    var_high /= num_high;

    return var_low*w1 + var_high*w2;

}

///Otsu threshold with minimizing the in class variance
int otsu(Mat src){
    int* histo = calculate_histogram(src);
    int bestT; float bestTF = INFINITY;
    for(int i = 0; i < 256; i++){
        float f = calculate_otsu_threshold(histo, i);
        if(f < bestTF) {bestTF = f, bestT = i;}
    }

    free(histo);
    return bestT;
}

///Otsu threshold with maximizing the between class variance
int otsu2(Mat src){
    ///init starting values
    int* histo = calculate_histogram(src);
    float q_current, u_current, best_var = 0;
    q_current = histo[0]/(src.rows * src.cols);
    u_current = 0.0f;
    int u;
    for (int i = 0; i < 256; i++)
        u += i*histo[i];
    int th = 0;

    ///find the largest variance between classes
    for(int i = 1; i < 256; i++){
        float u1_new, u2_new, q_new;
        q_new = q_current + ((float)histo[i])/(src.rows * src.cols);
        u1_new = (q_new == 0)? 0 : ((u_current* q_current) + i*histo[i])/q_new;
//        if(q_new == 0) u1_new = 0;
//        else u1_new = ((u_current* q_current) + i*histo[i])/q_new;
        u2_new = u - q_new * u1_new;
        u2_new /= 1 - q_new;
        float var = q_new * (1 - q_new) * pow(u1_new - u2_new,2);
        if(var > best_var) {
            best_var = var;
            th = i;
        }
        q_current = q_new;
        u_current = u1_new;
    }
    free(histo);
    return th;
}

int main() {
    Mat img = imread("/Users/somaizsombor/egyetem/Image_processing/proiect_test_images/test4.bmp", IMREAD_GRAYSCALE);

    ///Calculate Otsu threshold
    int otsuThreshold = otsu2(img);



    ///Calculate intensities
    bin_tree* binarizationData = binarization(img);
    std::vector<int> thresholds = binarizationData->getValues();
    std::vector<Highlight_T> highlights;

    printf("Otsu threshold: %d\n", otsuThreshold);
    Mat otsuImg(img.rows, img.cols, CV_8UC1);
    greyScaleToBlackAndWhite(img, otsuImg, otsuThreshold);
    imshow("Otsu", otsuImg);

    printf("Thresholds: ");
    binarizationData -> inorder();
    //head -> inorder();
    printf("\nIntensities: ");
    binarizationData -> inorder_color();
    printf("\n");

    ///Generate recursive image
    std::vector<int> intensities = binarizationData -> getColors();
    int lookUpTable[256];
    for(int i = 0; i <= thresholds.size(); i++){
        for(int j = (i > 0)? thresholds.at(i - 1): 0; j <= ((i >= thresholds.size())? 255:thresholds.at(i)); j++){
            lookUpTable[j] = intensities.at(i);
        }
    }


    Mat output(img.rows, img.cols, CV_8UC1);
    for(int i = 0; i< img.rows; i++)
        for(int j = 0; j< img.cols; j++)
            output.at<uchar>(i,j) = lookUpTable[img.at<uchar>(i,j)];
    imshow("Recursive segmentation", output);

    int* histo2 = calculate_histogram(output);
    showHistogram("Recursive segmentation histogram", histo2, 255, 256);

    ///Generate histogram
    int*histo = calculate_histogram(img);
    highlights.push_back(Highlight_T(otsuThreshold, Vec3b(255, 255,0 )));
    for(int t: thresholds){
        highlights.push_back(Highlight_T(t, Vec3b(255, 0, 0)));
    }

    for(int c: intensities){
        highlights.push_back(Highlight_T(c, Vec3b(0, 153,  255)));
    }

    showHistogram("Histogram", histo,255, 256, highlights);
    imshow("Original", img);
    waitKey(0);

    free(histo);
    free(histo2);
    return 0;
}
