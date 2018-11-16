#include <chrono> 
#include <cstring>
using namespace std::chrono;
using namespace std;
#include "veb.hpp"

unsigned long long int mstCost = 0;

TvEB::TvEB ( int uniSize )
  : uni ( powTwoRoundUp ( uniSize ) ), uniSqrt ( sqrt ( uni ) ),
    lowerUniSqrt ( lowerSqrt ( uni ) ), higherUniSqrt ( higherSqrt ( uni ) ),
    min ( UNDEFINED ), max ( UNDEFINED ), summary ( NULL )
{
  if ( uniSize <= 0 )
  {
    std::cerr << "universe size of TvEB must be bigger than 0" << std::endl;
    return;
  }

  if ( uni > 2 )
  {
    cluster = new TvEB * [higherUniSqrt];
    for ( int i = 0; i < higherUniSqrt; ++i )
    {
      cluster[i] = NULL;
    }
  }
  else
  {
    cluster = NULL;
  }
}

TvEB::~TvEB()
{
  if ( summary ) delete summary;
  if ( cluster )
  {
    for ( int i = 0; i < higherUniSqrt; ++i )
    {
      if ( cluster[i] ) delete cluster[i];
    }
    delete [] cluster;
  }
}

int powTwoRoundUp ( int x )
{
  if ( x < 0 ) return 0;
  --x;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  return x + 1;
}

float lowerSqrt ( int val )
{
  return pow ( 2, floor ( log2 ( val )  / 2 ) );
}

float higherSqrt ( int val )
{
  return pow ( 2, ceil ( log2 ( val )  / 2 ) );
}

int low ( TvEB * tree, int val )
{
  return val % ( int ) lowerSqrt ( tree->uni );
}

int high ( TvEB * tree, int val )
{
  return val / lowerSqrt ( tree->uni );
}

int index ( TvEB * tree, int high, int low )
{
  return high * lowerSqrt ( tree->uni ) + low;
}

bool vEB_min ( TvEB * tree, int & res )
{
  if ( tree )
  {
    res = tree->min;
    return true;
  }
  return false;
}

bool vEB_max ( TvEB * tree, int & res )
{
  if ( tree )
  {
    res = tree->max;
    return true;
  }
  return false;
}

bool vEB_insert ( TvEB *& tree, int val, int parentUniSqrt )
{
  if ( !tree )
  {
    tree = new TvEB ( parentUniSqrt );
  }

#ifdef DEBUG
  DEBUG_OS << "inserting " << val << " to tree " << tree
           << " of size " << tree->uni << DEBUG_OS_ENDL;
#endif /* DEBUG */

  if ( val < 0 || val >= tree->uni ) return false;

  if ( tree->min == val || tree->max == val ) return false;

  if ( tree->min == UNDEFINED )
  {
    tree->min = tree->max = val;
    return true;
  }

  if ( val < tree->min )
  {
    int tmp = val;
    val = tree->min;
    tree->min = tmp;
  }

  if ( val > tree->max )
  {
    tree->max = val;
  }

  if ( tree->uni > 2 )
  {
    int lowVal = low ( tree, val );
    int highVal = high ( tree, val );
    if ( !tree->cluster[highVal] )
    {
      if ( !vEB_insert ( tree->summary, highVal, tree->higherUniSqrt ) ) return false;
    }

    if ( !vEB_insert ( tree->cluster[highVal], lowVal, tree->lowerUniSqrt ) ) return false;
  }
  return true;
}

bool vEB_delete ( TvEB *& tree, int val )
{
  if ( !tree ) return false;

#ifdef DEBUG
  DEBUG_OS << "deleting " << val << " from tree " << tree
           << " of size " << tree->uni << DEBUG_OS_ENDL;
#endif /* DEBUG */

  if ( val < 0 || val >= tree->uni ) return false;
  if ( tree->min > val || tree->max < val ) return false;

  if ( tree->min == val )
  {
    int i;
    if ( !vEB_min ( tree->summary, i ) || i == UNDEFINED )
    {
      if ( tree->min != tree->max )
      {
        tree->min = tree->max;
        return true;
      }

      tree->min = tree->max = UNDEFINED;
      delete tree;
      tree = NULL;
      return true;
    }

    val = tree->min = index ( tree, i, tree->cluster[i]->min );
  }

  if ( tree->uni > 2 )
  {
    int highVal = high ( tree, val );
    if ( !vEB_delete ( tree->cluster[highVal], low ( tree, val ) ) ) return false;

    int tmp;
    if ( !vEB_min ( tree->cluster[highVal], tmp ) || tmp == UNDEFINED )
    {
      if ( !vEB_delete ( tree->summary, highVal ) ) return false;
    }
  }

  if ( tree->max == val )
  {
    int tmp;
    if ( !vEB_max ( tree->summary, tmp ) || tmp == UNDEFINED )
    {
      tree->max = tree->min;
    }
    else
    {
      int i;
      if ( !vEB_max ( tree->summary, i ) ) return false;
      tree->max = index ( tree, i, tree->cluster[i]->max );
    }
  }
  return true;
}

bool vEB_find ( TvEB * tree, int val )
{
  if ( !tree ) return false;

#ifdef DEBUG
  DEBUG_OS << "looking for " << val << " in tree " << tree << DEBUG_OS_ENDL;
#endif /* DEBUG */

  if ( val < 0 || val >= tree->uni ) return false;
  if ( tree->min > val || tree->max < val ) return false;
  if ( tree->min == val ) return true;
  if ( !tree->summary )
  {
    return tree->max == val;
  }
  if ( !vEB_find ( tree->cluster[high ( tree, val )], low ( tree, val ) ) )
    return false;
  return true;
}

bool vEB_succ ( TvEB * tree, int val, int & res )
{
  if ( !tree ) return false;

#ifdef DEBUG
  DEBUG_OS << "looking for successor of " << val << " in tree " << tree
           << " of size " << tree->uni << DEBUG_OS_ENDL;
#endif /* DEBUG */

  if ( val < -1 || val >= tree->uni ) return false;

  if ( tree->min > val )
  {
    res = tree->min;
    return true;
  }

  if ( !tree->summary )
  {
    if ( tree->max > val )
    {
      res = tree->max;
      return true;
    }
    return false;
  }

  int lowVal = low ( tree, val );
  int highVal = high ( tree, val );
  int i = highVal;
  int j = UNDEFINED;
  int tmp;
  if ( vEB_max ( tree->cluster[i], tmp ) && lowVal < tmp )
  {
    if ( !vEB_succ ( tree->cluster[i], lowVal, j ) ) return false;
  }
  else
  {
    if ( !vEB_succ ( tree->summary, highVal, i ) )
    {
      if ( tree->max > val )
      {
        res = tree->max;
        return true;
      }
      return false;
    }
    if ( !vEB_min ( tree->cluster[i], j ) ) return false;
  }

  res = index ( tree, i, j );
  return true;
}

bool vEB_pred ( TvEB * tree, int val, int & res )
{
  if ( !tree ) return false;

#ifdef DEBUG
  DEBUG_OS << "looking for predecessor of " << val << " in tree " << tree
           << " of size " << tree->uni << DEBUG_OS_ENDL;
#endif /* DEBUG */

  if ( val < 0 || val > tree->uni ) return false;

  if ( tree->max < val )
  {
    res = tree->max;
    return true;
  }

  if ( !tree->summary )
  {
    if ( tree->min < val )
    {
      res = tree->min;
      return true;
    }
    return false;
  }

  int lowVal = low ( tree, val );
  int highVal = high ( tree, val );
  int i = highVal;
  int j = UNDEFINED;
  int tmp;
  if ( vEB_min ( tree->cluster[i], tmp ) && lowVal > tmp )
  {
    if ( !vEB_pred ( tree->cluster[i], lowVal, j ) ) return false;
  }
  else
  {
    if ( !vEB_pred ( tree->summary, highVal, i ) )
    {
      if ( tree->min < val )
      {
        res = tree->min;
        return true;
      }
      return false;
    }
    if ( !vEB_max ( tree->cluster[i], j ) ) return false;
  }

  res = index ( tree, i, j );
  return true;
}

void vEB_print_1 ( TvEB * tree, std::ostream & os )
{
  if ( !tree ) return;
  os << "tree: " << tree << std::endl;
  os << "min: " << tree->min << ", max: " << tree->max << std::endl;
  os << "uni: " << tree->uni << ", uniSqrt: " << tree->uniSqrt << std::endl;
  os << "lowerUniSqrt: " << tree->lowerUniSqrt;
  os << ", higherUniSqrt: " << tree->higherUniSqrt << std::endl;
  os << "summary: " << tree->summary << std::endl;
  if ( tree->uni > 2 )
  {
    for ( int i = 0; i < tree->higherUniSqrt; ++i )
    {
      os << "cluster " << i << ": " << tree->cluster[i] << std::endl;
    }
  }
  else
  {
    os << "cluster " << tree->cluster << std::endl;
  }
}

void vEB_print ( TvEB * tree )
{
  if ( !tree ) return;
  cout << "tree: " << tree << std::endl;
  cout << "min: " << tree->min << ", max: " << tree->max << std::endl;
  cout << "uni: " << tree->uni << ", uniSqrt: " << tree->uniSqrt << std::endl;
  cout << "lowerUniSqrt: " << tree->lowerUniSqrt;
  cout << ", higherUniSqrt: " << tree->higherUniSqrt << std::endl;
  cout << "summary: " << tree->summary << std::endl;
  if ( tree->uni > 2 )
  {
    /*for ( int i = 0; i < tree->higherUniSqrt; ++i )
    {
      cout << "cluster " << i << ": " << tree->cluster[i] << std::endl;
    }*/
  }
  else
  {
    cout << "cluster " << tree->cluster << std::endl;
  }
}



void computeMST(const char * sfile,const char *  dfile)
{
  int res;
  int universe = 1000000;
  int testCnt = 0;
  int failedTestsCnt = 0;
	TvEB * tree = new TvEB ( universe );
 

    freopen(sfile,"r",stdin);
    freopen(dfile,"w",stdout);
    long long int maximumValue=INT_MIN;
    int n;
    cin>>n;
    vector<pair<int,int>> vec[n];
    string line;
    int s,d,wt;
	
    int rnode;
	  rnode=0;
    
	
    /*int minWieghtofRandomNode = INT_MAX;
	  int minWieghtSource = -1;
	  int minWieghtDestination = -1;*/
  int ec=0;
   bool torunRest=true;

    while(true)
    {
      
        cin>>s;
        if(cin.fail()==true)
        break;
        cin>>d;
        cin>>wt;
        if(torunRest==false)
          vEB_insert ( tree, wt );
        //
        //if(wt==821){
        //    cout<<s<<" is ourdce for wt  is 821 has destn = "<<d<<endl;
        //}
        vec[s].push_back(make_pair(d,wt));
        vec[d].push_back(make_pair(s,wt));
	
	    if(maximumValue<wt)
		    maximumValue = wt;
    }
   
   
    if(torunRest==false){
		  vEB_print(tree);
		  return;
	  }

	 map <int, pair<int,int>> mapofEdges;
	 vector<pair<int,pair<int,int>>> DuplicateEdgesToInsert;
	  
	
  int visited[100000];
  memset(visited,0,sizeof(visited));
   // int rnode = 0;
  int i;int cnt=1;
  int v;
	int minimumEdgeWeight ,source ,destination;
	//cout << " sassw " <<  n <<  " " << cnt << endl;
    while(cnt<n)
    {
        if(destination==-1){
            cout<<" MST not possible "<<endl;
            return ;
        }
        pair<int,int> par;
        visited[rnode] = 1;
	      //cout << "TEST cnt " <<cnt<< endl;
        for(i=0; i<vec[rnode].size();i++)
	      {
                 par = vec[rnode][i];
                if(vEB_find ( tree, par.second )==false)
                {
                  vEB_insert ( tree, par.second );
                  mapofEdges[par.second] = make_pair(rnode,par.first);
                }
                else
                {
                  DuplicateEdgesToInsert.push_back(make_pair(par.second,make_pair(rnode,par.first)));
                }
		
            pair<int,int> tofind;
            tofind = make_pair(rnode,par.second);
            vector<pair<int,int>>::iterator itr;
            itr = find(vec[par.first].begin(),vec[par.first].end(),tofind);
            if(itr != vec[par.first].end())
	          {
                    vec[par.first].erase(itr);
            }
        }
	
	      vEB_min ( tree, minimumEdgeWeight );

        source = mapofEdges[minimumEdgeWeight].first;
        destination = mapofEdges[minimumEdgeWeight].second;

          //cout << "destination 1 : " << destination << endl;

	      mapofEdges[minimumEdgeWeight]=make_pair(-1,-1);

	bool result = vEB_delete ( tree, minimumEdgeWeight );
	
	for(int i=0;i<DuplicateEdgesToInsert.size();i++)
	{
		int toInsertWeight = DuplicateEdgesToInsert[i].first;
		if(vEB_find ( tree, toInsertWeight )==false)
		{
			vEB_insert ( tree, toInsertWeight );
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
		        mstCost+=minimumEdgeWeight;
            cout<<"edge weight " << minimumEdgeWeight <<endl;
            cout<<"mst edge "<<source<<" "<<destination<<endl;
		 cnt++;
        }    
        else{
            while(visited[destination]==1 && DuplicateEdgesToInsert.size()>0)
	          {
	
              vEB_min ( tree, minimumEdgeWeight );
              
              source = mapofEdges[minimumEdgeWeight].first;
              destination = mapofEdges[minimumEdgeWeight].second;

               //cout << "destination 2 : " << destination << endl;

              mapofEdges[minimumEdgeWeight]=make_pair(-1,-1);
              vEB_delete ( tree, minimumEdgeWeight );

              for(int i=0;i<DuplicateEdgesToInsert.size();i++)
              {
                int toInsertWeight = DuplicateEdgesToInsert[i].first;
                
                if(vEB_find ( tree, toInsertWeight )==false)
                {
                  
                  //V = vEB_tree_insert(V, toInsertWeight, 1, u);
                  vEB_insert ( tree, toInsertWeight );
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
            }
		if(visited[destination]==0)
		{
			visited[destination] = 1;
			visited[source] =1;
			mstCost+=minimumEdgeWeight;
      cout<<"edge weight " << minimumEdgeWeight <<endl;
      cout<<"mst edge "<<source<<" "<<destination<<endl;
			cnt++;
		}
        }
        rnode = destination;
    }


  
}



int main ( int argc, char ** argv )
{
	
  if(argc<3){
    cout<<"invalid parameters"<<endl;
    return 0;
  }
  const char *  sfile=(argv[1]);
  const char *  dfile=(argv[2]);
   auto start = high_resolution_clock::now();
  computeMST(sfile,dfile);
  
   auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stop - start); 
 
    cout << duration.count() << endl;
	cout << "mst cost is : " << mstCost << endl;
  return 0;
}

