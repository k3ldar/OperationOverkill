#include "Queue.h"

Queue::Queue(int capacity)
{
	Serial.print("Create Queue: ");
	Serial.println(capacity);
	
    _capacity = capacity;
	_size = 0;
}
 
// Queue is full when size
// becomes equal to the capacity
bool Queue::isFull()
{
    return (_size == _capacity);
}
 
// Queue is empty when size is 0
bool Queue::isEmpty()
{
    return (_size == 0);
}

// Function to add an item to the queue.
// It changes rear and size
void Queue::enqueue(int item)
{
    if (isFull())
        return;
	
    _size++;
	QItem* temp = new QItem(item);
	
	if (_rear == NULL)
	{
		_front = _rear = temp;
		return;
	}
	
    _rear->next = temp;
	_rear = temp;
}
 
int Queue::dequeue()
{
    if (isEmpty())
        return INT_MIN;
        
    QItem* temp = _front;
	_front = _front->next;
    _size--;
	
	if (_front == NULL)
		_rear = NULL;
	
	int value = temp->data;
	delete(temp);
	
    return value;
}
 
int Queue::average()
{
    if (isEmpty())
	{
        return INT_MIN;
	}
        
    int sum = 0;
 
	QItem* temp = _front;
	do
	{
		sum += temp->data;
		temp = temp->next;
	}
	while (temp != NULL);
	
    return (sum / _size);
}
