#include <iostream> 
#include <queue> 
#include <algorithm>
#include <queue>
#include <cstring>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <chrono> 
using namespace std::chrono;
using namespace std; 
  
enum COLOR { RED, BLACK }; 
  
class Node { 
public: 
  pair<int,pair<int,int>> val;
  COLOR color; 
  Node *left, *right, *parent; 
  
  Node(pair<int,pair<int,int>> val) : val(val) { 
    parent = left = right = NULL; 
  
    // Node is created during insertion 
    // Node is red at insertion 
    color = RED; 
  } 
  
  // returns pointer to uncle 
  Node *uncle() { 
    // If no parent or grandparent, then no uncle 
    if (parent == NULL or parent->parent == NULL) 
      return NULL; 
  
    if (parent->isOnLeft()) 
      // uncle on right 
      return parent->parent->right; 
    else
      // uncle on left 
      return parent->parent->left; 
  } 
  
  // check if node is left child of parent 
  bool isOnLeft() { return this == parent->left; } 
  
  // returns pointer to sibling 
  Node *sibling() { 
    // sibling null if no parent 
    if (parent == NULL) 
      return NULL; 
  
    if (isOnLeft()) 
      return parent->right; 
  
    return parent->left; 
  } 
  
  // moves node down and moves given node in its place 
  void moveDown(Node *nParent) { 
    if (parent != NULL) { 
      if (isOnLeft()) { 
        parent->left = nParent; 
      } else { 
        parent->right = nParent; 
      } 
    } 
    nParent->parent = parent; 
    parent = nParent; 
  } 
  
  bool hasRedChild() { 
    return (left != NULL and left->color == RED) or 
           (right != NULL and right->color == RED); 
  } 
}; 
  
class RBTree { 
  Node *root; 
  
  
  // left rotates the given node 
  void leftRotate(Node *x) { 
    // new parent will be node's right child 
    Node *nParent = x->right; 
  
    // update root if current node is root 
    if (x == root) 
      root = nParent; 
  
    x->moveDown(nParent); 
  
    // connect x with new parent's left element 
    x->right = nParent->left; 
    // connect new parent's left element with node 
    // if it is not null 
    if (nParent->left != NULL) 
      nParent->left->parent = x; 
  
    // connect new parent with x 
    nParent->left = x; 
  } 
  
  void rightRotate(Node *x) { 
    // new parent will be node's left child 
    Node *nParent = x->left; 
  
    // update root if current node is root 
    if (x == root) 
      root = nParent; 
  
    x->moveDown(nParent); 
  
    // connect x with new parent's right element 
    x->left = nParent->right; 
    // connect new parent's right element with node 
    // if it is not null 
    if (nParent->right != NULL) 
      nParent->right->parent = x; 
  
    // connect new parent with x 
    nParent->right = x; 
  } 
  
  void swapColors(Node *x1, Node *x2) { 
    COLOR temp; 
    temp = x1->color; 
    x1->color = x2->color; 
    x2->color = temp; 
  } 
  
  void swapValues(Node *u, Node *v) { 
    pair<int,pair<int,int>> temp; 
    temp = u->val; 
    u->val = v->val; 
    v->val = temp; 
  } 
  
  // fix red red at given node 
  void fixRedRed(Node *x) { 
    // if x is root color it black and return 
    if (x == root) { 
      x->color = BLACK; 
      return; 
    } 
  
    // initialize parent, grandparent, uncle 
    Node *parent = x->parent, *grandparent = parent->parent, 
         *uncle = x->uncle(); 
  
    if (parent->color != BLACK) { 
      if (uncle != NULL && uncle->color == RED) { 
        // uncle red, perform recoloring and recurse 
        parent->color = BLACK; 
        uncle->color = BLACK; 
        grandparent->color = RED; 
        fixRedRed(grandparent); 
      } else { 
        // Else perform LR, LL, RL, RR 
        if (parent->isOnLeft()) { 
          if (x->isOnLeft()) { 
            // for left right 
            swapColors(parent, grandparent); 
          } else { 
            leftRotate(parent); 
            swapColors(x, grandparent); 
          } 
          // for left left and left right 
          rightRotate(grandparent); 
        } else { 
          if (x->isOnLeft()) { 
            // for right left 
            rightRotate(parent); 
            swapColors(x, grandparent); 
          } else { 
            swapColors(parent, grandparent); 
          } 
  
          // for right right and right left 
          leftRotate(grandparent); 
        } 
      } 
    } 
  } 
  
  // find node that do not have a left child 
  // in the subtree of the given node 
  Node *successor(Node *x) { 
    Node *temp = x; 
  
    while (temp->left != NULL) 
      temp = temp->left; 
  
    return temp; 
  } 
  
  // find node that replaces a deleted node in BST 
  Node *BSTreplace(Node *x) { 
    // when node have 2 children 
    if (x->left != NULL and x->right != NULL) 
      return successor(x->right); 
  
    // when leaf 
    if (x->left == NULL and x->right == NULL) 
      return NULL; 
  
    // when single child 
    if (x->left != NULL) 
      return x->left; 
    else
      return x->right; 
  } 
  
  // deletes the given node 
  void deleteNode(Node *v) { 
    Node *u = BSTreplace(v); 
  
    // True when u and v are both black 
    bool uvBlack = ((u == NULL or u->color == BLACK) and (v->color == BLACK)); 
    Node *parent = v->parent; 
  
    if (u == NULL) { 
      // u is NULL therefore v is leaf 
      if (v == root) { 
        // v is root, making root null 
        root = NULL; 
      } else { 
        if (uvBlack) { 
          // u and v both black 
          // v is leaf, fix double black at v 
          fixDoubleBlack(v); 
        } else { 
          // u or v is red 
          if (v->sibling() != NULL) 
            // sibling is not null, make it red" 
            v->sibling()->color = RED; 
        } 
  
        // delete v from the tree 
        if (v->isOnLeft()) { 
          parent->left = NULL; 
        } else { 
          parent->right = NULL; 
        } 
      } 
      delete v; 
      return; 
    } 
  
    if (v->left == NULL or v->right == NULL) { 
      // v has 1 child 
      if (v == root) { 
        // v is root, assign the value of u to v, and delete u 
        v->val = u->val; 
        v->left = v->right = NULL; 
        delete u; 
      } else { 
        // Detach v from tree and move u up 
        if (v->isOnLeft()) { 
          parent->left = u; 
        } else { 
          parent->right = u; 
        } 
        delete v; 
        u->parent = parent; 
        if (uvBlack) { 
          // u and v both black, fix double black at u 
          fixDoubleBlack(u); 
        } else { 
          // u or v red, color u black 
          u->color = BLACK; 
        } 
      } 
      return; 
    } 
  
    // v has 2 children, swap values with successor and recurse 
    swapValues(u, v); 
    deleteNode(u); 
  } 
  
  void fixDoubleBlack(Node *x) { 
    if (x == root) 
      // Reached root 
      return; 
  
    Node *sibling = x->sibling(), *parent = x->parent; 
    if (sibling == NULL) { 
      // No sibiling, double black pushed up 
      fixDoubleBlack(parent); 
    } else { 
      if (sibling->color == RED) { 
        // Sibling red 
        parent->color = RED; 
        sibling->color = BLACK; 
        if (sibling->isOnLeft()) { 
          // left case 
          rightRotate(parent); 
        } else { 
          // right case 
          leftRotate(parent); 
        } 
        fixDoubleBlack(x); 
      } else { 
        // Sibling black 
        if (sibling->hasRedChild()) { 
          // at least 1 red children 
          if (sibling->left != NULL and sibling->left->color == RED) { 
            if (sibling->isOnLeft()) { 
              // left left 
              sibling->left->color = sibling->color; 
              sibling->color = parent->color; 
              rightRotate(parent); 
            } else { 
              // right left 
              sibling->left->color = parent->color; 
              rightRotate(sibling); 
              leftRotate(parent); 
            } 
          } else { 
            if (sibling->isOnLeft()) { 
              // left right 
              sibling->right->color = parent->color; 
              leftRotate(sibling); 
              rightRotate(parent); 
            } else { 
              // right right 
              sibling->right->color = sibling->color; 
              sibling->color = parent->color; 
              leftRotate(parent); 
            } 
          } 
          parent->color = BLACK; 
        } else { 
          // 2 black children 
          sibling->color = RED; 
          if (parent->color == BLACK) 
            fixDoubleBlack(parent); 
          else
            parent->color = BLACK; 
        } 
      } 
    } 
  } 
  
  // prints level order for given node 
  void levelOrder(Node *x) { 
    if (x == NULL) 
      // return if node is null 
      return; 
  
    // queue for level order 
    queue<Node *> q; 
    Node *curr; 
  
    // push x 
    q.push(x); 
  
    while (!q.empty()) { 
      // while q is not empty 
      // dequeue 
      curr = q.front(); 
      q.pop(); 
  
      // print node value 
      cout << (curr->val).first << " "; 
  
      // push children to queue 
      if (curr->left != NULL) 
        q.push(curr->left); 
      if (curr->right != NULL) 
        q.push(curr->right); 
    } 
  } 
  
  // prints inorder recursively 
  void inorder(Node *x) { 
    if (x == NULL) 
      return; 
    inorder(x->left); 
    cout << (x->val).first << " "; 
    inorder(x->right); 
  } 
  
public: 
  // constructor 
  // initialize root 
  RBTree() { root = NULL; } 
  
  Node *getRoot() { return root; } 

  int searchbyval(int valtosearch){
    Node *temp = root; 
    int f=0;
    while (temp != NULL) { 
      if (valtosearch < (temp->val).first) { 
        if (temp->left == NULL) 
          break; 
        else
          temp = temp->left; 
      } 
      else if (valtosearch ==(temp->val).first) { 
        f =1;
        break; 
        
      } 
      else { 
        if (temp->right == NULL) 
          break; 
        else
          temp = temp->right; 
      } 
    } 
  
    return f;
   
    }
  // searches for given value 
  // if found returns the node (used for delete) 
  // else returns the last node while traversing (used in insert) 
  Node *search(pair<int,pair<int,int>> n) { 
    Node *temp = root; 
    while (temp != NULL) { 
      if (n < temp->val) { 
        if (temp->left == NULL) 
          break; 
        else
          temp = temp->left; 
      } else if (n == temp->val) { 
        break; 
      } else { 
        if (temp->right == NULL) 
          break; 
        else
          temp = temp->right; 
      } 
    } 
  
    return temp; 
  } 
  

  // inserts the given value to tree 
  void insert(pair<int,pair<int,int>> n) { 
    Node *newNode = new Node(n); 
    if (root == NULL) { 
      // when root is null 
      // simply insert value at root 
      newNode->color = BLACK; 
      root = newNode; 
    } else { 
      Node *temp = search(n); 
  
      if (temp->val == n) { 
        // return if value already exists 
        return; 
      } 
  
      // if value is not found, search returns the node 
      // where the value is to be inserted 
  
      // connect new node to correct node 
      newNode->parent = temp; 
  
      if (n < temp->val) 
        temp->left = newNode; 
      else
        temp->right = newNode; 
  
      // fix red red voilaton if exists 
      fixRedRed(newNode); 
    } 
  } 
  
  // utility function that deletes the node with given value 
  void deleteByVal(pair<int,pair<int,int>> n) { 
    if (root == NULL) 
      // Tree is empty 
      return; 
  
    Node *v = search(n), *u; 
  
    if (v->val != n) { 
      cout << "No node found to delete with value:" << n.first << endl; 
      return; 
    } 
  
    deleteNode(v); 
  } 
  
  // prints inorder of the tree 
  void printInOrder() { 
    cout << "Inorder: " << endl; 
    if (root == NULL) 
      cout << "Tree is empty" << endl; 
    else
      inorder(root); 
    cout << endl; 
  } 
  
  // prints level order of the tree 
  void printLevelOrder() { 
    cout << "Level order: " << endl; 
    if (root == NULL) 
      cout << "Tree is empty" << endl; 
    else
      levelOrder(root); 
    cout << endl; 
  } 

  pair<int,pair<int,int>> findmin(Node * r){
    pair<int,pair<int,int>> p;
    Node * temp = r;
    if(temp==NULL)
        return p;
    if(temp->left)
        return findmin(temp->left);
    return temp->val;
    
   }
}; 
long long mstcost=0;
int main ( int argc, char ** argv ){
    if(argc<3){
    cout<<"invalid parameters"<<endl;
    return 0;
    }
    RBTree tree; 
    const char *  sfile=(argv[1]);
    const char *  dfile=(argv[2]);
    auto start = high_resolution_clock::now();
   
    freopen(sfile,"r",stdin);
    freopen(dfile,"w",stdout);
    int n;
    cin>>n;
    vector<pair<int,int>> vec[n];
    string line;
    int s,d,wt;
    int rnode;
	  rnode=0;
    while(true){
        cin>>s;
        if(cin.fail()==true)
            break;
        cin>>d;
        cin>>wt;
        vec[s].push_back(make_pair(d,wt));
        vec[d].push_back(make_pair(s,wt));
        //cout<<wt<<endl;
    }
    map <int, pair<int,int>> mapofEdges;
	  vector<pair<int,pair<int,int>>> DuplicateEdgesToInsert;
    int visited[1000000];
    memset(visited,0,sizeof(visited));
    int minimumEdgeWeight ,source ,destination;
    int i;int cnt=1;
    int v;
    pair<int,pair<int,int>> mini;
    while(cnt<n){
        if(destination==-1){
            cout<<" MST not possible "<<endl;
            return 0;
        }
        pair<int,int> par;
        visited[rnode] = 1;
        for(i=0; i<vec[rnode].size();i++){
            par = vec[rnode][i];
            //cout<<"asd 1"<<endl;
            if(tree.searchbyval(par.second)==0){
                tree.insert(make_pair(par.second,make_pair(rnode,par.first)));
                mapofEdges[par.second] = make_pair(rnode,par.first);

                //cout<<"for weight "<<par.second<<" src "<<rnode<<" dtn "<<
                //par.first<<endl;
                
            }
            else
            {
                  DuplicateEdgesToInsert.push_back(make_pair(par.second,make_pair(rnode,par.first)));
            }
            pair<int,int> tofind;
            tofind = make_pair(rnode,par.second);

            //cout<<"rnode "<<rnode<<"wt par.sec "<<par.second<<" par.fir "<<par.first<<endl;
            vector<pair<int,int>>::iterator itr;
            itr = find(vec[par.first].begin(),vec[par.first].end(),tofind);
            if(itr != vec[par.first].end()){
                    vec[par.first].erase(itr);
            }
        }
        //cout<<"inorder is "<<endl;
        //inorder(root);
        //cout<<endl;
        mini = tree.findmin(tree.getRoot());
        minimumEdgeWeight = mini.first;
        tree.deleteByVal( mini);
        source = mapofEdges[minimumEdgeWeight].first;
        destination = mapofEdges[minimumEdgeWeight].second;
	      mapofEdges[minimumEdgeWeight]=make_pair(-1,-1);
        
        for(int i=0;i<DuplicateEdgesToInsert.size();i++)
        {
            int toInsertWeight = DuplicateEdgesToInsert[i].first;
            if(tree.searchbyval(toInsertWeight)==0)
            {
                tree.insert(make_pair(toInsertWeight,make_pair(rnode,par.first)));

                //cout<<"to insrt wt "<<toInsertWeight<<" src "<<DuplicateEdgesToInsert[i].second.first<<" dtn "<<
                //DuplicateEdgesToInsert[i].second.second<<endl;

                mapofEdges[toInsertWeight] = 			
                make_pair(DuplicateEdgesToInsert[i].second.first,
                DuplicateEdgesToInsert[i].second.second);
                int x = DuplicateEdgesToInsert[i].second.first;
                int y = DuplicateEdgesToInsert[i].second.second;
                pair<int,pair<int,int>> tof = make_pair(toInsertWeight,make_pair(x,y));
                vector<pair<int,pair<int,int>>>::iterator itr;
                itr = find(DuplicateEdgesToInsert.begin(),DuplicateEdgesToInsert.end(),tof);
                if(itr != DuplicateEdgesToInsert.end()){
                    DuplicateEdgesToInsert.erase(itr);
                }
            }
	    }

        if(visited[destination]==0){
            visited[destination] = 1;
            visited[source] =1;
            mstcost+=minimumEdgeWeight;
            cout<<"edge weight " << minimumEdgeWeight <<endl;
            cout<<"mst edge "<<source<<" "<<destination<<endl;
            cnt++;
        }    
        else{
            while(visited[destination]==1 && DuplicateEdgesToInsert.size()>0)
	          {
                    //cout<<"inorder is "<<endl;
                    //inorder(root);
                    //cout<<endl;
                    mini = tree.findmin(tree.getRoot());
                    minimumEdgeWeight = mini.first;
                    tree.deleteByVal( mini);
                    source = mapofEdges[minimumEdgeWeight].first;
                    destination = mapofEdges[minimumEdgeWeight].second;
                    mapofEdges[minimumEdgeWeight]=make_pair(-1,-1);
             
                    for(int i=0;i<DuplicateEdgesToInsert.size();i++){
                      int toInsertWeight = DuplicateEdgesToInsert[i].first;
                      if(tree.searchbyval(toInsertWeight)==0){
                        tree.insert(make_pair(toInsertWeight,make_pair(rnode,par.first)));
                        mapofEdges[toInsertWeight] = 			
                        make_pair(DuplicateEdgesToInsert[i].second.first,
                        DuplicateEdgesToInsert[i].second.second);
                        int x = DuplicateEdgesToInsert[i].second.first;
                        int y = DuplicateEdgesToInsert[i].second.second;
                        pair<int,pair<int,int>> tof = make_pair(toInsertWeight,make_pair(x,y));
                        vector<pair<int,pair<int,int>>>::iterator itr;
                        itr = find(DuplicateEdgesToInsert.begin(),DuplicateEdgesToInsert.end(),tof);
                        if(itr != DuplicateEdgesToInsert.end())
                            DuplicateEdgesToInsert.erase(itr);
                        
                }
              }
            }
            if(visited[destination]==0){
              visited[destination] = 1;
              visited[source] =1;
              mstcost+=minimumEdgeWeight;
              cout<<"edge weight " << minimumEdgeWeight <<endl;
              cout<<"mst edge "<<source<<" "<<destination<<endl;
              cnt++;
            }
        }
        
        rnode = destination;
    }
    auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stop - start); 
    cout << duration.count() << endl<<"mst cost is "<< mstcost<<endl; 
    return 0;
}


