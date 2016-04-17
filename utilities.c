#include "utilities.h"

int parser(char *buffer, char ***elem, char sep)
{
	int i=0, endElem=0, startElem=0, nbElems=0, numElem = 0;
	while(buffer[i] != '\n' && buffer[i] != '\0')
	{
		if(buffer[i] == sep)
		{
			while(buffer[i] == sep)
			{
				i++;
			}
		}
		else
		{
			nbElems++;
			while(buffer[i] != sep && buffer[i] != '\n' && buffer[i] != '\0')
			{
				i++;
			}
		}
	}
	*elem = malloc(sizeof(char *)*(nbElems+1));
	i=0;
	do
	{
		if(buffer[i] == sep)
		{
			if(buffer[i-1] == '\\') // Si le séparateur est échappé
			{
				strcpy(&(buffer[i-1]), &(buffer[i]));
			}
			else
			{
				while(buffer[i] == sep)
				{
					i++;
				}
			}
		}
		else
		{
			startElem = i;
			while(buffer[i] != sep && buffer[i] != '\n' && buffer[i] != '\0')
			{
				i++;
			}
			endElem = i;
			(*elem)[numElem] = malloc(sizeof(char)*endElem-startElem);
			strncpy((*elem)[numElem], &(buffer[startElem]), endElem-startElem);
			(*elem)[numElem][endElem-startElem] = '\0';
			numElem++;
		}
	}while(buffer[i] != '\n' && buffer[i] != '\0');
	(*elem)[numElem] = NULL;
	return nbElems;
}
