#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <queue>
using namespace std;
// color = 0 => red
// color = 1 => black
enum color {RED, BLACK};
struct rbnode{
    int data;
    bool color;
    struct rbnode * left;
    struct rbnode * right;
    struct rbnode * parent; 
};
struct rbnode * leftRotate(struct rbnode * node){
    struct rbnode * newroot = node->right;
    struct rbnode * temp = newroot->left;

    
    //update parents
    if(node->parent)
        if(node->parent->left == node)
            node->parent->left = newroot;
        else
            node->parent->right = newroot;
    newroot->parent = node->parent;
    node->parent = newroot;
    if(temp)
        temp->parent = node;
    //rotate
    newroot->left = node;
    node->right = temp;
    return node;
}
struct rbnode * rightRotate(struct rbnode * node){
    struct rbnode * newroot = node->left;
    struct rbnode * temp = newroot->right;

    
    //update parents
    if(node->parent)
        if(node->parent->left == node)
            node->parent->left = newroot;
        else
            node->parent->right = newroot;
    newroot->parent = node->parent;
    node->parent = newroot;
    if(temp)
        temp->parent = node;
    
    //rotate
    newroot->right = node;
    node->left = temp;
    return node;
}
struct rbnode * fixup(struct rbnode * t , struct rbnode * z){
    cout<<"received z is "<<z->data<<z->color<<endl;
    cout<<"z parent is "<<z->parent->data<<endl;
    cout<<"z parent color is "<<z->parent->color<<endl;
    if(z->parent)
    while(z->parent && z->parent->color == RED){
        cout<<"inside"<<endl;
        if(z->parent == z->parent->parent->left){ // check if it is left or right child of parent
            struct rbnode * uncle = z->parent->parent->right;
            if(!uncle){
                z->parent->color = BLACK;
                z=z->parent;
            }
            else if(uncle && uncle->color == RED){
                z->parent->color = BLACK;
                uncle->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else if(z == z->parent->right){
                z = z->parent;
                z = leftRotate(z);
            }
            else{
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                z = rightRotate(z->parent->parent);
                z = z->parent;
            }
        } 
        else { // z is right child of parent
            struct rbnode * uncle = z->parent->parent->left;
            if(!uncle){
                z->parent->color = BLACK;
                z=z->parent;
            }
            else if(uncle && uncle->color == RED){
                z->parent->color = BLACK;
                uncle->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else if(z == z->parent->left){
                z = z->parent;
                z = rightRotate(z);
            }
            else {
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                z = leftRotate(z->parent->parent);
                z = z->parent;
            }
        }
    }
    if(!z->parent){
        t = z;
    }
    t->color = BLACK;
    return t;
}
struct rbnode * insert(struct rbnode * root ,int value){
    struct rbnode * temp =(struct rbnode *) malloc(sizeof(struct rbnode));
    temp->data = value;
    temp->color = RED;
    temp->left = NULL;
    temp->right = NULL;
    temp->parent = NULL;
    
    struct rbnode * follow = NULL;
    struct rbnode * rootcopy = root;
    while(rootcopy){
        follow =rootcopy;
        if(rootcopy->data > temp->data)
            rootcopy = rootcopy->left;
        else
            rootcopy = rootcopy->right;
    }
   
    if(!follow){
        temp->color = BLACK;
        return temp;
    }
    else if(follow->data > temp->data){
        follow->left = temp;
        
    }
    else if(follow->data < temp->data){
        follow->right = temp;
    }
    else{
        return root;   
    }
    temp->parent = follow;
    cout<<"follow is "<<follow->data<<follow->color<<endl;
    if(!root)
        root = follow;
    //cout<<"root is "<<root<<endl;
    root = fixup(root , temp);
   // cout<<"after fixup root is "<<root->data<<endl;
    return root;
     
}
void inorder(struct rbnode * node){
    if(node->left)
        inorder(node->left);
    printf("%d ",node->data);
    if(node->right)
        inorder(node->right);
}
void levelorder (struct rbnode *root){
    struct rbnode * temp = root;
    queue<struct rbnode *> q;
    q.push(temp);
    while(!q.empty()){
        temp = q.front();
        q.pop();
        cout<<temp->data<<"-"<<temp->color<<" ";
        if(temp->left)
            q.push(temp->left);
        if(temp->right)
            q.push(temp->right);
    }
}
int main(){
    struct rbnode * root = NULL;
    /*root = insert(root, 10);
    //cout<<" root->data "<<root->data<<" "<<root->color<<endl;
    root = insert(root, 20);
    
    //cout<<" root->data "<<root->data<<endl;
    root = insert(root, 30);
    
     //cout<<" root->data "<<root->data<<endl;
    root = insert(root, 40);
    //inorder(root);
    // cout<<" root->data "<<root->data<<endl;
    root = insert(root, 50);
     //cout<<" root->data "<<root->data<<endl;*/
    root = insert(root, 7);
    root = insert(root, 3);
    root = insert(root, 18);
    root = insert(root, 10);
    root = insert(root, 22);
    root = insert(root, 8);
    root = insert(root, 11);
    root = insert(root, 26);
    root = insert(root, 2);
    root = insert(root, 6);
    root = insert(root, 13);

    inorder(root);cout<<endl;
    levelorder(root);
    return 0;
}