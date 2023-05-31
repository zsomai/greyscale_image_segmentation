//
// Created by Somai Zsombor on 26.03.2023.
//

#ifndef PROJECT_GREYSCALE_SEGMENTATION_BIN_TREE_H
#define PROJECT_GREYSCALE_SEGMENTATION_BIN_TREE_H
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>

class bin_tree {
    private:
        int val;
        std::vector<int> colorsAsigned;
        bin_tree* left;
        bin_tree* right;
    public:
        bin_tree(int i);
        bin_tree();
        void inorder();
        void inorder_color();
        std::vector<int> getColors();
        std::vector<int> getValues();
        void setRight(int v);
        void setLeft(int v);
        void setRight(bin_tree* v);
        void setLeft(bin_tree* v);
        void setVal(int v);
        void setColors(std::vector<int> colors);
        bin_tree* getRight();
        bin_tree* getLeft();
        int getVal();
        std::vector<int> getColorsAsigned();
        void deleteTree();
        bin_tree* getLeftest();
        bin_tree* getRightest();
};


#endif //PROJECT_GREYSCALE_SEGMENTATION_BIN_TREE_H
