#include "cmds.h"

int parser(char *buffer, char ***elem, char sep)
{
	int i, endElem=0, startElem=0, nbElems=1, numElem = 0;
	for(i=0;buffer[i]!='\n';i++)
	{
		if(buffer[i] == sep)
		{
			nbElems++;
		}
	}
	*elem = malloc(sizeof(char *)*(nbElems+1));
	do
	{
		for(i=startElem;buffer[i]!='\n';i++)
		{
			if(buffer[i] == sep)
			{
				if(buffer[i-1] == '\\')
				{
					strcpy(&(buffer[i-1]), &(buffer[i]));
				}
				else
				{
					break;
				}
			}	
		}
		endElem = i;
		(*elem)[numElem] = malloc(sizeof(char)*endElem-startElem);
		strncpy((*elem)[numElem], &(buffer[startElem]), endElem-startElem);
		(*elem)[numElem][endElem-startElem] = '\0';
		numElem++;
		startElem = endElem+1;
	}while(buffer[i] != '\n');
	(*elem)[numElem] = NULL;
}
