#include <cmath>
#include <climits>
#include <cstdlib>
#include <cstdio>
#include <iostream>

#include <bits/stdc++.h>
using namespace std;

// #define DEBUG
#define DEBUG_OS std::cout
#define DEBUG_OS_ENDL std::endl

#define UNDEFINED INT_MIN
struct TvEB
{
  TvEB ( int uniSize );
  ~TvEB();
  const int uni;
  const int uniSqrt;
  const int lowerUniSqrt;
  const int higherUniSqrt;
  int min;
  int max;
  TvEB * summary;
  TvEB ** cluster;
};
int powTwoRoundUp ( int val );
float lowerSqrt ( int val );
float higherSqrt ( int val );
int low ( TvEB * tree, int val );
int high ( TvEB * tree, int val );
int index ( TvEB * tree, int high, int low );
bool vEB_min ( TvEB * tree, int & res );
bool vEB_max ( TvEB * tree, int & res );
bool vEB_insert ( TvEB *& tree, int val, int parentUniSqrt = 65536 );
bool vEB_delete ( TvEB *& tree, int val );
bool vEB_find ( TvEB * tree, int val );
bool vEB_succ ( TvEB * tree, int val, int & res );
bool vEB_pred ( TvEB * tree, int val, int & res );
void vEB_print ( TvEB * tree );
