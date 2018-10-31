#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <queue>
using namespace std;
struct avlnode{
    int data;
    struct avlnode * left;
    struct avlnode * right;
    int height;                 //helpful for balance factor calculation
};
int findHeight(struct avlnode * node){
    if(!node)
        return 0;
    else
        return node->height;
}
struct avlnode * leftRotate (struct avlnode * node){
    struct avlnode * newRoot = node->right;
    struct avlnode * temp = newRoot->left;

    //Rotate
    newRoot->left = node;
    node->right = temp;

    //assign new heights
   
    node->height = 1 + max(findHeight(node->left),findHeight(node->right));
    newRoot->height = 1 + max(findHeight(newRoot->left),findHeight(newRoot->right));

    return newRoot;
}
struct avlnode * rightRotate (struct avlnode * node){
    struct avlnode * newRoot = node->left;
    struct avlnode * temp = newRoot->right;

    //Rotate
    newRoot->right = node;
    node->left = temp;

    //assign new heights
    node->height = 1 + max(findHeight(node->left),findHeight(node->right));
    newRoot->height = 1 + max(findHeight(newRoot->left),findHeight(newRoot->right));

    return newRoot;
}
struct avlnode * insert(struct avlnode *node ,int value){
    if(node==NULL){
        struct avlnode * temp =(struct avlnode *)malloc(sizeof(struct avlnode ));
        temp->data = value;
        temp->left = NULL;
        temp->right = NULL;
        temp->height = 1;
        return temp;
    }
    
    if(node->data > value)
        node->left = insert(node->left, value);
    else if(node->data <= value)
        node->right = insert(node->right, value);
    //else   
        //return node;
    
    node->height = 1 + max(findHeight(node->left),findHeight(node->right));
   
    int balanceFactor = node? (findHeight(node->left) - findHeight(node->right)):0;
    
    // left left rotation
    if(balanceFactor > 1 && value < node->left->data )
        return rightRotate(node);
    // left right rotation
    if(balanceFactor > 1 && value > node->left->data ){
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    // right right rotation
    if(balanceFactor < -1 && value > node->right->data)
        return leftRotate(node);
    // right left rotation    
    if(balanceFactor < -1 && value < node->right->data){
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}
void inorder(struct avlnode * node){
    if(node->left)
        inorder(node->left);
    printf("%d ",node->data);
    if(node->right)
        inorder(node->right);
}
struct avlnode * findInorderSuccessor(struct avlnode * node){
    if(node->left)
        return findInorderSuccessor(node->left);
    return node;
}
struct avlnode * deleteNode(struct avlnode * node ,int value){
    if(!node)
        return NULL;
    if(node->data > value)
        deleteNode(node->left , value);
    else if(node->data <= value)
        deleteNode(node->right ,value);
    else{
        struct avlnode *temp = node;
        if(node->right == NULL && node->left == NULL){
            node = NULL;
            free(temp);
            
        }
        else if(node->right == NULL && node->left != NULL){
            node = node->left;
            free(temp);
        }
        else if(node->right != NULL && node->left == NULL){
            node = node->right;
            free(temp);
        }
        else{
            temp = findInorderSuccessor(node->right);
            cout<<"inoder succ of "<<node->data<<" is "<<temp->data<<endl;
            node->data = temp->data;
            node->right = deleteNode(node->right , temp->data);
        }
    }
    if(!node)
        return node;
    node->height = 1 + max(findHeight(node->left),findHeight(node->right));
    int balanceFactor = node? (findHeight(node->left) - findHeight(node->right)):0;
    int leftBalance = node->left ? (findHeight(node->left->left) - findHeight(node->left->right)):0;
    int rightBalance = node->right ? (findHeight(node->right->left) - findHeight(node->right->right)):0;
    //left left case
    if(balanceFactor > 1 && leftBalance >= 0)
        return rightRotate(node);
    if(balanceFactor > 1 && leftBalance < 0){
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if(balanceFactor < -1 && rightBalance <= 0)
        leftRotate(node);
     if(balanceFactor < -1 && rightBalance > 0){
         node->right = rightRotate(node->right);
         return leftRotate(node);
     }
     return node;
}
void levelorder (struct avlnode *root){
    struct avlnode * temp = root;
    queue<struct avlnode *> q;
    q.push(temp);
    while(!q.empty()){
        temp = q.front();
        q.pop();
        cout<<temp->data<<" ";
        if(temp->left)
            q.push(temp->left);
        if(temp->right)
            q.push(temp->right);
    }
}
int main(){
    struct avlnode * root = NULL;
    root = insert(root, 10);
    root = insert(root, 20);
    root = insert(root, 5);
    root = insert(root, 30);
    root = insert(root, 40);
    root = insert(root, 50);
    root = insert(root, 30);
    inorder(root);
    cout<<endl;
    levelorder(root);
    cout<<endl;
    root = deleteNode(root, 30);
    inorder(root);
    cout<<endl;
    cout<<endl;
    levelorder(root);
    return 0;
}