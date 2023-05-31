//
// Created by Somai Zsombor on 26.03.2023.
//

#include "bin_tree.h"

void bin_tree::inorder() {
    if(left != nullptr)
        left -> inorder();

    printf("%d ", val);
    if(right != nullptr)
        right -> inorder();
}

void bin_tree::inorder_color() {
    if(left != nullptr)
        left -> inorder_color();
    for(int color : colorsAsigned){
        printf("%d ", color);
    }
    if(right != nullptr)
        right -> inorder_color();
}

void bin_tree::setRight(int v) {
    right = new bin_tree(v);
}

void bin_tree::setLeft(int v) {
    left = new bin_tree(v);
}

void bin_tree::setVal(int v) {
    val = v;
}

void bin_tree::setColors(std::vector<int> colors) {

    colorsAsigned = colors;
}

bin_tree* bin_tree::getRight() {
    return right;
}

bin_tree* bin_tree::getLeft() {
    return left;
}

int bin_tree::getVal() {
    return val;
}

std::vector<int> bin_tree::getColorsAsigned() {
    return colorsAsigned;
}

void bin_tree::deleteTree() {
    if(left != nullptr)
        left -> deleteTree();
    if(right != nullptr)
        right -> deleteTree();
    free(this);
}

void bin_tree::setRight(bin_tree *v) {
    right = v;
}

void bin_tree::setLeft(bin_tree *v) {
    left = v;
}

bin_tree::bin_tree() {
    left = nullptr;
    right = nullptr;
}

bin_tree::bin_tree(int i) {
    left = nullptr;
    right = nullptr;
    val = i;
}

bin_tree *bin_tree::getLeftest() {
    if(this -> left == nullptr) return  this;
    bin_tree* current = this -> left;
    while(current -> left != nullptr) current = current -> left;
    return current;
}

bin_tree *bin_tree::getRightest() {
    if(this -> right == nullptr) return  this;
    bin_tree* current = this -> right;
    while(current -> right != nullptr) current = current -> right;
    return current;}

std::vector<int> bin_tree::getValues() {
    std::vector<int> v1, v2;
    if (this -> getLeft() != nullptr)
        v1 = this -> getLeft() -> getValues();
    if(this -> getRight() != nullptr)
        v2 = this -> getRight() -> getValues();

    v1.push_back(this -> val);
    v1.insert(v1.end(), v2.begin(), v2.end());
    return v1;
}

std::vector<int> bin_tree::getColors() {
    std::vector<int> v1, v2;
    if (this -> getLeft() != nullptr)
        v1 = this -> getLeft() -> getColors();
    if(this -> getRight() != nullptr)
        v2 = this -> getRight() -> getColors();

    v1.insert(v1.end(), this -> colorsAsigned.begin(), this->colorsAsigned.end());
    v1.insert(v1.end(), v2.begin(), v2.end());
    return v1;
}
