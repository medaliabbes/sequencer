/*
* The MIT License (MIT)
*
* Copyright (c) 2024 Mohamed Ali Ben Abbes
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#include "sequencer_queue.h"


bool Queue_Init(queue_t * Queue , uint8_t * buffer , uint8_t size)
{
  if( (buffer == NULL) || (size == 0) || (Queue == NULL))
  {
    return false ;
  }

  Queue->buffer   = buffer ;
  Queue->size     = size   ;
  Queue->WritePtr = buffer ;
  return true ;
}

bool Queue_Push(queue_t * Queue , Q_Event_t  element)
{
	/** This condition to prevent crashes*/
  if(Queue == NULL)
  {
    return false ;
  }

  if(Queue->WritePtr >= (Queue->buffer + Queue->size))
  {
    return false ;
  }
  *(uint8_t*)(Queue->WritePtr) = element.id ;
  Queue->WritePtr++ ;
  *(uint8_t*)(Queue->WritePtr) = element.priority ;
  Queue->WritePtr++ ;

  return true ;
}

bool Queue_Pop(queue_t * Queue , Q_Event_t * element)
{
  /** This condition to prevent crashes*/
  if(Queue == NULL)
  {
    return false ;
  }

  if(Queue->WritePtr <= Queue->buffer )
  {
    return false ;
  }
  element->priority = *(uint8_t*)(Queue->WritePtr - 1) ;
  Queue->WritePtr-- ;
  element->id = *(uint8_t*)(Queue->WritePtr - 1) ;
  Queue->WritePtr-- ;
  return true ;
}

bool Queue_Dump(queue_t * Queue )
{
  /** This condition to prevent crashes*/
  if(Queue == NULL)
  {
    return false ;
  }

  Queue->WritePtr = Queue->buffer ;
  return true ;
}

bool Queue_Is_Epmty(queue_t * Queue)
{
  /** This condition to prevent crashes*/
  if(Queue == NULL)
  {
    return false ;
  }

  if(Queue->WritePtr == Queue->buffer)
  {
    return true ;
  }
  return false ;
}

int  Queue_Get_Size(queue_t * Queue)
{
  /** This condition to prevent crashes*/
  if(Queue == NULL)
  {
    return false ;
  }
  int size = (Queue->WritePtr - Queue->buffer) / sizeof(Q_Event_t);
  return size ;
}

bool Queue_Sort(queue_t * Queue) 
{
  if(Queue == NULL)
  {
    return false ;
  }

  int queue_size = Queue_Get_Size(Queue) ;
  if((queue_size % 2 != 0) || (queue_size == 0))
    return false ;
  
  Q_Event_t * event_array = (Q_Event_t *) Queue->buffer ;
  Q_Event_t aux ;
  for(int i = 0 ; i < queue_size - 1  ; i++)
  {
    for(int j = i+1 ; j < queue_size  ; j++)
    {
      if(event_array[j].priority < event_array[i].priority)
      {
        /**swap */
        aux            = event_array[j] ;
        event_array[j] = event_array[i] ;
        event_array[i] = aux ;
      }
    }
  }
  return true ;
}

bool Queue_Deinit(queue_t * Queue)
{
  /** This condition to prevent crashes*/
  if(Queue == NULL)
  {
    return false ;
  }

  Queue->buffer   = NULL ;
  Queue->size     = 0    ;
  Queue->WritePtr = NULL ;
  return true ;
}
