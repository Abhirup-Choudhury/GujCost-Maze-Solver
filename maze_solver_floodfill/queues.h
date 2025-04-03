#ifndef Queues_H_
#define Queues_H_
// not using templates since templating in arduino is f***ed up
// hardcoding the Cell data type.
// Datatype to store Cell values and Points in queue.
struct Point // 2D coordinate
{
    short x;
    short y;

    bool operator==(const Point &other) const
    {
        return x == other.x && y == other.y;
    }
    bool operator!=(const Point &other) const
    {
        return x != other.x or y != other.y;
    }
};

struct Cell // combination of Point and floodValue
{
    Point point;
    short floodValue;
    // Overloading the == operator for the Cell struct
    bool operator==(const Cell &other) const
    {
        return point == other.point && floodValue == other.floodValue;
    }
};

// QUEUE=============================START
class Queue
{
private:
  short qSize;
  Cell* arr;
  short rear, front;
  short itemCount;
public:
  Queue(short size)
  {
    front = rear = -1;
    itemCount = 0;
    qSize = size;
    arr = new Cell[qSize];
    // Cell arr[qSize];
  }

  // Access operator for indexing using operator overloading.
  Cell &operator[](short index)
  {
    if (index < 0 || index >= itemCount)
    {
      Serial.print("%d index is out of range.\n");
    }
    return arr[(front + index) % qSize];
  }

  bool isEmpty()
  {
    return front == -1;
  }

  bool isFull()
  {
    return ((front == 0 && rear == qSize - 1) || (rear == front - 1));
  }

  void push(Cell value)
  {
    if (front == -1)
    {
      front = rear = 0;
      arr[rear] = value;
    }
    else if (rear == qSize - 1 && front != 0)
    {
      rear = 0;
      arr[rear] = value;
    }
    else
    {
      rear++;
      arr[rear] = value;
    }
    itemCount++;
  }

  Cell pop()
  {
    short oldFront = front;
    if (front == rear)
    {
      front = rear = -1;
    }
    else if (front == qSize - 1)
      front = 0;
    else
      front++;

    itemCount--;
    return arr[oldFront];
  }

  // Return the index of an element's first occurence if present,
  // otherwise return -1.
  short find(Cell key)
  {
    for (int i = 0; i < itemCount; i++)
    {
      if ((*this)[i] == key)
        return i;
    }

    return -1;
  }

  // flush the queue after round is over.
  void flush()
  {
    itemCount = 0;
    front = rear = -1;
  }

  // Get number of items present in the queue.
  short getItemCount()
  {
    return itemCount;
  }

  // Get the maximum queue size
  short getMaxSize()
  {
    return qSize;
  }
};
//QUEUE===========================END

#endif /* Queues_H_ */
