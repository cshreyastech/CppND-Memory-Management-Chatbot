#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>

#include "chatlogic.h"
#include "graphnode.h"
#include "graphedge.h"
#include "chatbot.h"

// constructor WITHOUT memory allocation
ChatBot::ChatBot()
{
    // invalidate data handles
    _image = nullptr;
    _chatLogic = nullptr;
    _rootNode = nullptr;
}

// constructor WITH memory allocation
ChatBot::ChatBot(std::string filename)
{
    std::cout << "ChatBot Constructor" << std::endl;
    
    // invalidate data handles
    _chatLogic = nullptr;
    _rootNode = nullptr;

    // load image into heap memory
    _image = new wxBitmap(filename, wxBITMAP_TYPE_PNG);
}

// 1. Destructor: Frees the dynamically allocated memory
// This is called automatically when an object of this class goes out of scope.
ChatBot::~ChatBot()
{
    std::cout << "ChatBot Destructor" << std::endl;

    // deallocate heap memory
    if(_image != NULL) // Attention: wxWidgets used NULL and not nullptr
    {
        delete _image;
        _image = NULL;
    }
}

//// STUDENT CODE
////
// 2. Copy Constructor: Creates a new object as a deep copy of another.
// It is called when an object is initialized with another object of the same type.
// E.g., MyDynamicClass newObj = oldObj;
ChatBot::ChatBot(const ChatBot& other)
{
  std::cout << "Copy constructor called. Performing a deep copy." << std::endl;
  
  _image = new wxBitmap(*other._image);

  _currentNode = other._currentNode;
  _rootNode = other._rootNode;
  _chatLogic = other._chatLogic;
}

// 3. Copy Assignment Operator: Assigns one object's value to another.
// It is called when an object is assigned to another.
// E.g., existingObj = otherObj;
ChatBot& ChatBot::operator=(const ChatBot& other)
{
  std::cout << "Copy assignment operator called." << std::endl;
  if (this != &other) {
    delete _image;
    delete _currentNode;
    delete _rootNode;
    delete _chatLogic;

    _image = new wxBitmap(*other._image);
    _currentNode = other._currentNode;
    _rootNode = other._rootNode;
    _chatLogic = other._chatLogic;
  }

  return *this;
}

// 4. Move Constructor: Steals the resources from another object.
// The 'other' parameter is an rvalue reference (&&).
ChatBot::ChatBot(ChatBot&& other) noexcept
{
  std::cout << "Move constructor called. Stealing resources." << std::endl;
  // Steal the pointer from the source object.
  _image = other._image;
  _currentNode = other._currentNode;
  _rootNode = other._rootNode;
  _chatLogic = other._chatLogic;
  
  
  // Null out the source's pointer so its destructor doesn't free the memory.
  other._image = nullptr;
  other._currentNode = nullptr;
  other._rootNode = nullptr;
  other._chatLogic = nullptr;
}

// 5. Move Assignment Operator: Steals the resources from another object.
ChatBot& ChatBot::operator=(ChatBot&& other) noexcept
{
  std::cout << "Move assignment operator called." << std::endl;
  // Check for self-assignment.
  if (this != &other) {
    // First, free our own memory.
    delete _image;
    delete _currentNode;
    delete _rootNode;
    delete _chatLogic;
    
    // Then, steal the pointer from the source.
    _image = other._image;
    _currentNode = other._currentNode;
    _rootNode = other._rootNode;
    _chatLogic = other._chatLogic;

    // Null out the source's pointer.
    other._image = nullptr;
    other._currentNode = nullptr;
    other._rootNode = nullptr;
    other._chatLogic = nullptr;
  }
  return *this;
}

////
//// EOF STUDENT CODE

void ChatBot::ReceiveMessageFromUser(std::string message)
{
    // loop over all edges and keywords and compute Levenshtein distance to query
    typedef std::pair<GraphEdge *, int> EdgeDist;
    std::vector<EdgeDist> levDists; // format is <ptr,levDist>

    for (size_t i = 0; i < _currentNode->GetNumberOfChildEdges(); ++i)
    {
        GraphEdge *edge = _currentNode->GetChildEdgeAtIndex(i);
        for (auto keyword : edge->GetKeywords())
        {
            EdgeDist ed{edge, ComputeLevenshteinDistance(keyword, message)};
            levDists.push_back(ed);
        }
    }

    // select best fitting edge to proceed along
    GraphNode *newNode;
    if (levDists.size() > 0)
    {
        // sort in ascending order of Levenshtein distance (best fit is at the top)
        std::sort(levDists.begin(), levDists.end(), [](const EdgeDist &a, const EdgeDist &b) { return a.second < b.second; });
        newNode = levDists.at(0).first->GetChildNode(); // after sorting the best edge is at first position
    }
    else
    {
        // go back to root node
        newNode = _rootNode;
    }

    // tell current node to move chatbot to new node
    _currentNode->MoveChatbotToNewNode(newNode);
}

void ChatBot::SetCurrentNode(GraphNode *node)
{
    // update pointer to current node
    _currentNode = node;

    // select a random node answer (if several answers should exist)
    std::vector<std::string> answers = _currentNode->GetAnswers();
    std::mt19937 generator(int(std::time(0)));
    std::uniform_int_distribution<int> dis(0, answers.size() - 1);
    std::string answer = answers.at(dis(generator));

    // send selected node answer to user
    _chatLogic->SendMessageToUser(answer);
}

int ChatBot::ComputeLevenshteinDistance(std::string s1, std::string s2)
{
    // convert both strings to upper-case before comparing
    std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);

    // compute Levenshtein distance measure between both strings
    const size_t m(s1.size());
    const size_t n(s2.size());

    if (m == 0)
        return n;
    if (n == 0)
        return m;

    size_t *costs = new size_t[n + 1];

    for (size_t k = 0; k <= n; k++)
        costs[k] = k;

    size_t i = 0;
    for (std::string::const_iterator it1 = s1.begin(); it1 != s1.end(); ++it1, ++i)
    {
        costs[0] = i + 1;
        size_t corner = i;

        size_t j = 0;
        for (std::string::const_iterator it2 = s2.begin(); it2 != s2.end(); ++it2, ++j)
        {
            size_t upper = costs[j + 1];
            if (*it1 == *it2)
            {
                costs[j + 1] = corner;
            }
            else
            {
                size_t t(upper < corner ? upper : corner);
                costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
            }

            corner = upper;
        }
    }

    int result = costs[n];
    delete[] costs;

    return result;
}