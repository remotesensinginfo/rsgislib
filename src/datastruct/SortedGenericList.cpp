/*
 *  SortedComparableList.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 15/04/2008.
 *  Copyright 2008 RSGISLib.
 * 
 *  RSGISLib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RSGISLib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SortedGenericList_H
#define SortedGenericList_H

#include <iostream>
#include <string>
#include "datastruct/RSGISItemNotFoundException.h"

namespace rsgis{ namespace datastruct{
	    
	template <typename T>
	class SortedGenericList
		{
		public: 
			SortedGenericList();
			SortedGenericList(int initSize);
			SortedGenericList(int initSize, int increment);
			void add(T *obj);
			T* peekTop();
			T* peekBottom();
			T* getAt(int index);
			void remove(T *obj) throw(RSGISItemNotFoundException);
			void removeNoDelete(T *obj) throw(RSGISItemNotFoundException);
			int getSize();
			void clearList();
			void clearListDelete();
			void printDesc();
			void startIterDesc();
			bool hasNextDesc();
			T* nextDesc();
			void printAsc();
			void startIterAsc();
			bool hasNextAsc();
			T* nextAsc();
			T* removeIterAsc();
			void removeNoDeleteIterAsc();
			T* getMostCommonValue();
			~SortedGenericList();
			void deleteData();
		private:
			void binaryChopSearch(const T *data, int *location);
			void increaseTableSize(int increaseBy);
			void bumpdown(int location);
			void bumpup(int location);
			T** list;
			int size;
			double notifyThreshold;
			int totalSize;
			int tableIncrement;
			int iteratorIndex;
		};

	template <typename T>
	SortedGenericList<T>::SortedGenericList()
	{
		totalSize = 50;
		tableIncrement = 50;
		size = 0;
		list = new T*[totalSize];
	}
	
	template <typename T>
	SortedGenericList<T>::SortedGenericList(int initSize)
	{
		totalSize = initSize;
		tableIncrement = 50;
		size = 0;
		list = new T*[totalSize];
	}
	
	template <typename T>
	SortedGenericList<T>::SortedGenericList(int initSize, int increment)
	{
		totalSize = initSize;
		tableIncrement = increment;
		size = 0;
		list = new T*[totalSize];
	}
	
	template <typename T>
	void SortedGenericList<T>::add(T *obj)
	{
		int location = 0;
		this->binaryChopSearch(obj, &location);
		// Need to bump down 
		this->bumpdown(location);
		// Insert
		list[location] = obj;
		//size++;
	}
	
	template <typename T>
	T*  SortedGenericList<T>::peekTop()
	{
		return list[0];
	}
	
	template <typename T>
	T*  SortedGenericList<T>::peekBottom()
	{
		return list[size-1];
	}
	
	template <typename T>
	T* SortedGenericList<T>::getAt(int index)
	{
		return list[index];
	}
	
	template <typename T>
	void SortedGenericList<T>::remove(T *obj) throw(RSGISItemNotFoundException)
	{
		int location = 0;
		this->binaryChopSearch(obj, &location);
		if(*obj == *list[location])
		{
			delete list[location];
			this->bumpup(location);
		}
		else
		{
			throw RSGISItemNotFoundException("Item could not be found within the list.");
		}
	}
	
	template <typename T>
	void SortedGenericList<T>::removeNoDelete(T *obj) throw(RSGISItemNotFoundException)
	{
		int location = 0;
		this->binaryChopSearch(obj, &location);
		if(*obj == *list[location])
		{
			this->bumpup(location);
		}
		else
		{
			throw RSGISItemNotFoundException("Item could not be found within the list.");
		}
	}
	
	template <typename T>
	int SortedGenericList<T>::getSize()
	{
		return size;
	}
	
	template <typename T>
	void SortedGenericList<T>::clearList()
	{
		for(int i = 0; i < size; i++)
		{
			list[i] = NULL;
		}
		size = 0;
	}
	
	template <typename T>
	void SortedGenericList<T>::clearListDelete()
	{
		for(int i = 0; i < size; i++)
		{
			if(list[i] != NULL)
			{
				delete list[i];
			}
		}
		size = 0;
	}
	
	template <typename T>
	void SortedGenericList<T>::printDesc()
	{
        std::cout << " ************ Descending (" << size << ")************* \n";
		if(size == 0)
		{
			std::cout << "List is empty!\n";
		}
		else if(list == NULL)
		{
			std::cout << "LIST IS NULL!\n";
		}
		else
		{
			for(int i = 0; i < size; i++)
			{
				std::cout << i << ") ";
				if(list[i] == NULL)
				{
					std::cout << "NULL\n";
				}
				else
				{
					std::cout << *list[i] << std::endl;
				}
			}
		}
	}
	
	template <typename T>
	void SortedGenericList<T>::startIterDesc()
	{
		iteratorIndex = 0;
	}
	
	template <typename T>
	bool SortedGenericList<T>::hasNextDesc()
	{
		bool returnValue = false;
		if(iteratorIndex < size)
		{
			returnValue = true;
		}
		
		return returnValue;
	}
	
	template <typename T>
	T* SortedGenericList<T>::nextDesc()
	{
		return list[iteratorIndex++]; 
	}
	
	template <typename T>
	void SortedGenericList<T>::printAsc()
	{
		std::cout << " ************ Ascending (" << size << ")************* \n";
		if(size == 0)
		{
			std::cout << "List is empty!\n";
		}
		else
		{
			int counter = 0;
			for(int i = size-1; i >= 0; i--)
			{
				std::cout << counter++ << ") ";
				if(list[i] == NULL)
				{
					std::cout << "NULL\n";
				}
				else
				{
					std::cout << *list[i] << std::endl;
				}
			}
		}
	}
	
	template <typename T>
	void SortedGenericList<T>::startIterAsc()
	{
		iteratorIndex = size-1;
	}
	
	template <typename T>
	bool SortedGenericList<T>::hasNextAsc()
	{
		bool returnValue = false;
		if(iteratorIndex >= 0)
		{
			returnValue = true;
		}
		
		return returnValue;
	}
	
	template <typename T>
	T* SortedGenericList<T>::nextAsc()
	{
		return list[iteratorIndex--]; 
	}
	
	template <typename T>
	T* SortedGenericList<T>::removeIterAsc()
	{
		delete list[iteratorIndex+1];
		this->bumpup(iteratorIndex+1);
	}
	
	template <typename T>
	void SortedGenericList<T>::removeNoDeleteIterAsc()
	{
		this->bumpup(iteratorIndex+1); 
	}
	
	template <typename T>
	T* SortedGenericList<T>::getMostCommonValue()
	{
		int maxCount = 0;
		T *value = NULL;
		
		int currentCount = 0;
		
		bool first = true;
		bool firstMax = true;
		
		for(int i = 0; i < size; i++)
		{
			if(first)
			{
				currentCount = 1;
				first = false;
			}
			else
			{
				if(list[i] == list[i-1])
				{
					currentCount++;
				}
				else
				{
					if(firstMax)
					{
						maxCount = currentCount;
						value = list[i-1];
						currentCount = 1;
						firstMax = false;
					}
					else
					{
						if(currentCount > maxCount)
						{
							maxCount = currentCount;
							value = list[i-1];
							currentCount = 1;
						}
						else
						{
							currentCount++;
						}
					}
				}
			}
		} 
		
		return value;
	}

	template <typename T>
	void SortedGenericList<T>::binaryChopSearch(const T *data, int *location)
	{
		int min = 0;
		int max = size-1;
		int midPoint = 0;
		bool continueLoop = true;
		
		if(size == 0)
		{
			*location = 0;
			continueLoop = false;
		}
		
		while(continueLoop)
		{
			if((max-min) < 2)
			{
				if(*data == *list[min])
				{
					*location = min;
					continueLoop = false;
					break;
				}
				else if(*data == *list[max])
				{
					*location = max;
					continueLoop = false;
					break;
				}
				else if(*data > *list[min]) // <
				{
					*location = min;
					continueLoop = false;
					break;
				}
				else if((*data < *list[min]) & // >
						(*data > *list[max])) // <
				{
					*location = min+1;
					continueLoop = false;
					break;
				}
				else if(*data < *list[max]) //>
				{
					*location = max+1;
					continueLoop = false;
					break;
				}
				
			}
			else
			{
				midPoint = min + static_cast<int>(((max-min)/2));
				if(*data == *list[midPoint])
				{
					*location = midPoint;
					continueLoop = false;
					break;
				}
				else if(*data > *list[midPoint]) // >
				{
					max = midPoint;
				}
				else
				{
					min = midPoint;
				}
			}
		}
	}
	
	template <typename T>
	void SortedGenericList<T>::increaseTableSize(int increaseBy)
	{
		// Create new table
		T** newTempTable = new T* [totalSize + increaseBy];
		
		//Copy data from old to new table
		for(int i = 0; i < size; i++)
		{
			newTempTable[i] = list[i];
			list[i] = NULL;
		}
		delete[] list;
		
		//Assign new table to old
		list = newTempTable;
		
		// Alter totalSize	
		totalSize = totalSize + increaseBy;
		//this->print();
	}
	
	template <typename T>
	void SortedGenericList<T>::bumpdown(int location)
	{
		int currentPointer = 0;
		if(size > 0)
		{
			currentPointer = size-1;
		}
		
		if(size+1 == totalSize)
		{
			this->increaseTableSize(this->tableIncrement);
		}
		
		while(currentPointer >= location)
		{
			list[currentPointer+1] = list[currentPointer];
			currentPointer--;
		}
		list[location] = NULL;
		size++;
	}
	
	template <typename T>
	void SortedGenericList<T>::bumpup(int location)
	{
		int currentPointer = location;
		while(currentPointer < size)
		{
			list[currentPointer] = list[currentPointer+1];
			currentPointer++;
		}
		list[currentPointer] = NULL;
		size--;
	}
	
	template <typename T>
	SortedGenericList<T>::~SortedGenericList()
	{
		delete[] list;
	}
	
	template <typename T>
	void SortedGenericList<T>::deleteData()
	{
		for(int i = 0; i < size; i++)
		{
			delete list[i];
		}
	}

}} //rsgis::datastruct

#endif

