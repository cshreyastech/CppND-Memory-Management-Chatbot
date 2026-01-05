#ifndef CHATBOT_H_
#define CHATBOT_H_

#include <wx/bitmap.h>
#include <string>

class GraphNode; // forward declaration
class ChatLogic; // forward declaration

class ChatBot
{
private:
  // data handles (owned)
  wxBitmap *_image{nullptr}; // avatar image

  // data handles (not owned)
  GraphNode *_currentNode{nullptr};
  GraphNode *_rootNode{nullptr};
  ChatLogic *_chatLogic{nullptr};

  // proprietary functions
  int ComputeLevenshteinDistance(std::string s1, std::string s2);

public:
  // constructors / destructors
  ChatBot();                     // constructor WITHOUT memory allocation
  ChatBot(std::string filename); // constructor WITH memory allocation

  // 1. Destructor: Frees the dynamically allocated memory
  // This is called automatically when an object of this class goes out of scope.
  ~ChatBot();

  //// STUDENT CODE
  ////
  
  // 2. Copy Constructor: Creates a new object as a deep copy of another.
  // It is called when an object is initialized with another object of the same type.
  // E.g., MyDynamicClass newObj = oldObj;
  ChatBot(const ChatBot& other);

  // 3. Copy Assignment Operator: Assigns one object's value to another.
  // It is called when an object is assigned to another.
  // E.g., existingObj = otherObj;
  ChatBot& operator=(const ChatBot& other);

  // 4. Move Constructor: Steals the resources from another object.
  // The 'other' parameter is an rvalue reference (&&).
  ChatBot(ChatBot&& other) noexcept;

  // 5. Move Assignment Operator: Steals the resources from another object.
  ChatBot& operator=(ChatBot&& other) noexcept;

  ////
  //// EOF STUDENT CODE

  // getters / setters
  void SetCurrentNode(GraphNode *node);
  void SetRootNode(GraphNode *rootNode) { _rootNode = rootNode; }
  void SetChatLogicHandle(ChatLogic *chatLogic) { _chatLogic = chatLogic; }
  ChatLogic* GetChatLogicHandle() { return _chatLogic; }
  wxBitmap *GetImageHandle() { return _image; }

  // communication
  void ReceiveMessageFromUser(std::string message);
};

#endif /* CHATBOT_H_ */
