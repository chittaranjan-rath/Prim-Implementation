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
  
class RBNode { 
public: 
  pair<int,pair<int,int>> val;
  COLOR color; 
  RBNode *left, *right, *parent; 
  
  RBNode(pair<int,pair<int,int>> val) : val(val) { 
    parent = left = right = NULL; 
    color = RED; 
  } 
  
  RBNode *uncle() { 
    if (parent == NULL or parent->parent == NULL) 
      return NULL; 
  
    if (parent->doesleftexist()) 
      return parent->parent->right; 
    else
      return parent->parent->left; 
  } 
  
  bool doesleftexist() { return this == parent->left; } 
  RBNode *sibling() { 
    if (parent == NULL) 
      return NULL; 
    if (doesleftexist()) 
      return parent->right; 
  
    return parent->left; 
  } 
  void moveDown(RBNode *nParent) { 
    if (parent != NULL) { 
      if (doesleftexist()) { 
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
  RBNode *root; 
  void leftRotate(RBNode *Node) { 
    RBNode *nParent = Node->right; 
    if (Node == root) 
      root = nParent; 
  
    Node->moveDown(nParent); 
    Node->right = nParent->left; 
    if (nParent->left != NULL) 
      nParent->left->parent = Node; 
    nParent->left = Node; 
  } 
  
  void rightRotate(RBNode *Node) { 
    RBNode *nParent = Node->left;  
    if (Node == root) 
      root = nParent; 
  
    Node->moveDown(nParent); 
  
    Node->left = nParent->right; 
    if (nParent->right != NULL) 
      nParent->right->parent = Node; 
  
    nParent->right = Node; 
  } 
  
  void swapColors(RBNode * node1, RBNode *node2) { 
    COLOR temp; 
    temp = node1->color; 
    node1->color = node2->color; 
    node2->color = temp; 
  } 
  
  void swapValues(RBNode * node1, RBNode * node2) { 
    pair<int,pair<int,int>> temp; 
    temp = node1->val; 
    node1->val = node2->val; 
    node2->val = temp; 
  } 
 
  void fixRedRed(RBNode *Node) { 
    if (Node == root) { 
      Node->color = BLACK; 
      return; 
    } 
    RBNode *parent = Node->parent, *grandparent = parent->parent, 
         *uncle = Node->uncle(); 
  
    if (parent->color != BLACK) { 
      if (uncle != NULL && uncle->color == RED) { 
        parent->color = BLACK; 
        uncle->color = BLACK; 
        grandparent->color = RED; 
        fixRedRed(grandparent); 
      } else { 
        if (parent->doesleftexist()) { 
          if (Node->doesleftexist()) { 
            swapColors(parent, grandparent); 
          } else { 
            leftRotate(parent); 
            swapColors(Node, grandparent); 
          }
          rightRotate(grandparent); 
        } else { 
          if (Node->doesleftexist()) { 
            rightRotate(parent); 
            swapColors(Node, grandparent); 
          } else { 
            swapColors(parent, grandparent); 
          } 
          leftRotate(grandparent); 
        } 
      } 
    } 
  } 

  RBNode *successor(RBNode *Node) { 
    RBNode *temp = Node; 
  
    while (temp->left != NULL) 
      temp = temp->left; 
  
    return temp; 
  } 
 
  RBNode *bstdelete(RBNode *Node) { 
    if (Node->left != NULL and Node->right != NULL) 
      return successor(Node->right); 
    if (Node->left == NULL and Node->right == NULL) 
      return NULL; 
    if (Node->left != NULL) 
      return Node->left; 
    else
      return Node->right; 
  } 
  void deleteRBNode(RBNode *v) { 
    RBNode *u = bstdelete(v); 
    bool uvBlack = ((u == NULL or u->color == BLACK) and (v->color == BLACK)); 
    RBNode *parent = v->parent; 
  
    if (u == NULL) { 
      if (v == root) { 
        root = NULL; 
      } else { 
        if (uvBlack) { 
          fixDoubleBlack(v); 
        } else { 
          if (v->sibling() != NULL) 
            v->sibling()->color = RED; 
        } 
        if (v->doesleftexist()) { 
          parent->left = NULL; 
        } else { 
          parent->right = NULL; 
        } 
      } 
      delete v; 
      return; 
    } 
  
    if (v->left == NULL or v->right == NULL) { 
      if (v == root) { 
        v->val = u->val; 
        v->left = v->right = NULL; 
        delete u; 
      } else { 
        if (v->doesleftexist()) { 
          parent->left = u; 
        } else { 
          parent->right = u; 
        } 
        delete v; 
        u->parent = parent; 
        if (uvBlack) { 
          fixDoubleBlack(u); 
        } else { 
          u->color = BLACK; 
        } 
      } 
      return; 
    } 
    swapValues(u, v); 
    deleteRBNode(u); 
  } 
  
  void fixDoubleBlack(RBNode *x) { 
    if (x == root) 
      return; 
  
    RBNode *sibling = x->sibling(), *parent = x->parent; 
    if (sibling == NULL) { 
      fixDoubleBlack(parent); 
    } else { 
      if (sibling->color == RED) { 
        parent->color = RED; 
        sibling->color = BLACK; 
        if (sibling->doesleftexist()) { 
          rightRotate(parent); 
        } else { 
          leftRotate(parent); 
        } 
        fixDoubleBlack(x); 
      } else { 
        if (sibling->hasRedChild()) { 
          if (sibling->left != NULL and sibling->left->color == RED) { 
            if (sibling->doesleftexist()) { 
              sibling->left->color = sibling->color; 
              sibling->color = parent->color; 
              rightRotate(parent); 
            } else { 
              sibling->left->color = parent->color; 
              rightRotate(sibling); 
              leftRotate(parent); 
            } 
          } else { 
            if (sibling->doesleftexist()) { 
              sibling->right->color = parent->color; 
              leftRotate(sibling); 
              rightRotate(parent); 
            } else { 
              sibling->right->color = sibling->color; 
              sibling->color = parent->color; 
              leftRotate(parent); 
            } 
          } 
          parent->color = BLACK; 
        } else { 
          sibling->color = RED; 
          if (parent->color == BLACK) 
            fixDoubleBlack(parent); 
          else
            parent->color = BLACK; 
        } 
      } 
    } 
  } 
  
  
public: 
  RBTree() { root = NULL; } 
  
  RBNode *findRoot() { return root; } 

  int searchbyval(int valtosearch){
    RBNode *temp = root; 
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
  RBNode *search(pair<int,pair<int,int>> node) { 
    RBNode *temp = root; 
    while (temp != NULL) { 
      if (node < temp->val) { 
        if (temp->left == NULL) 
          break; 
        else
          temp = temp->left; 
      } else if (node == temp->val) { 
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
  
  void insert(pair<int,pair<int,int>> n) { 
    RBNode *newRBNode = new RBNode(n); 
    if (root == NULL) { 
      newRBNode->color = BLACK; 
      root = newRBNode; 
    } else { 
      RBNode *temp = search(n); 
  
      if (temp->val == n) { 
        return; 
      } 
  
      newRBNode->parent = temp; 
  
      if (n < temp->val) 
        temp->left = newRBNode; 
      else
        temp->right = newRBNode; 
      fixRedRed(newRBNode); 
    } 
  } 
  
  void deleteByVal(pair<int,pair<int,int>> n) { 
    if (root == NULL) 
      return; 
  
    RBNode *v = search(n), *u; 
  
    if (v->val != n) { 
      cout << "No RBNode found to delete with value:" << n.first << endl; 
      return; 
    } 
  
    deleteRBNode(v); 
  } 
  void printLevelOrder() { 
    RBNode *x = root;
    cout << "Level order: " << endl; 
    if (root == NULL) 
      cout << "Tree is empty" << endl; 
    else{
      queue<RBNode *> q; 
      RBNode *curr; 
      q.push(x); 
      while (!q.empty()) { 
        curr = q.front(); 
        q.pop(); 
        cout << (curr->val).first << " "; 
        if (curr->left != NULL) 
          q.push(curr->left); 
        if (curr->right != NULL) 
          q.push(curr->right); 
      } 
    }
  } 

  pair<int,pair<int,int>> findmin(RBNode * r){
    pair<int,pair<int,int>> p;
    RBNode * temp = r;
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
    int rRBNode;
	  rRBNode=0;
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
        visited[rRBNode] = 1;
        for(i=0; i<vec[rRBNode].size();i++){
            par = vec[rRBNode][i];
            //cout<<"asd 1"<<endl;
            if(tree.searchbyval(par.second)==0){
                tree.insert(make_pair(par.second,make_pair(rRBNode,par.first)));
                mapofEdges[par.second] = make_pair(rRBNode,par.first);

                //cout<<"for weight "<<par.second<<" src "<<rRBNode<<" dtn "<<
                //par.first<<endl;
                
            }
            else
            {
                  DuplicateEdgesToInsert.push_back(make_pair(par.second,make_pair(rRBNode,par.first)));
            }
            pair<int,int> tofind;
            tofind = make_pair(rRBNode,par.second);

            //cout<<"rRBNode "<<rRBNode<<"wt par.sec "<<par.second<<" par.fir "<<par.first<<endl;
            vector<pair<int,int>>::iterator itr;
            itr = find(vec[par.first].begin(),vec[par.first].end(),tofind);
            if(itr != vec[par.first].end()){
                    vec[par.first].erase(itr);
            }
        }
        //cout<<"inorder is "<<endl;
        //inorder(root);
        //cout<<endl;
        mini = tree.findmin(tree.findRoot());
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
                tree.insert(make_pair(toInsertWeight,make_pair(rRBNode,par.first)));

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
                    mini = tree.findmin(tree.findRoot());
                    minimumEdgeWeight = mini.first;
                    tree.deleteByVal( mini);
                    source = mapofEdges[minimumEdgeWeight].first;
                    destination = mapofEdges[minimumEdgeWeight].second;
                    mapofEdges[minimumEdgeWeight]=make_pair(-1,-1);
             
                    for(int i=0;i<DuplicateEdgesToInsert.size();i++){
                      int toInsertWeight = DuplicateEdgesToInsert[i].first;
                      if(tree.searchbyval(toInsertWeight)==0){
                        tree.insert(make_pair(toInsertWeight,make_pair(rRBNode,par.first)));
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
        
        rRBNode = destination;
    }
    auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stop - start); 
    cout << duration.count() << endl<<"mst cost is "<< mstcost<<endl; 
    return 0;
}


