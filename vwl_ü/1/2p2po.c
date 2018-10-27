#include <stdio.h>
#include <stdlib.h>

/* Variants:
	producers: 2/n
	products: 2/n
	transformation curves: linear/marginal/any
	desires: same number for every good, fix number
		except 1 good, any possible formulation
*/

const static int PRODUCERS=2;
const static int PRODUCTS=2;

float prod_cost[2][2]={{16, 4}, {10, 5}};

int main(int argc, char** argv)
{
	int produced_by[PRODUCTS];
	float comp_cost[PRODUCERS][PRODUCTS];
	
	comp_cost[0][0]=prod_cost[0][0]/prod_cost[0][1];
	comp_cost[0][1]=prod_cost[0][1]/prod_cost[0][0];
	comp_cost[1][0]=prod_cost[1][0]/prod_cost[1][1];
	comp_cost[1][1]=prod_cost[1][1]/prod_cost[1][0];

	if(comp_cost[0][0]<comp_cost[1][0])
	{
		produced_by[0]=0;
		produced_by[1]=1;
	}
	else if(comp_cost[1][0]<comp_cost[0][0])
	{
		produced_by[0]=1;
		produced_by[1]=0;
	}

	return 0;
