#include <string>
#include <iostream>
#include <set>
#include <map>
#include <sstream>
#include <istream>
#include <algorithm>

enum Color {WHITE, GRAY, BLACK};

struct Node
{
  struct comp
  {
    bool operator() (Node* lh_node, Node* rh_node)
    {
      return lh_node->_id < rh_node->_id;
    }
  };

  Node* _parent;
  std::string _full_path;
  std::string _local_path;
  int _id;
  int _deep;
  Color _color;
  std::set<Node*, comp> _child;
  

  Node() :
    _parent(NULL),
    _full_path(""),
    _local_path(""),
    _id(0),
    _deep(0),
    _color(WHITE)
  {}

  ~Node()
  {}

  void set_white_color()
  {
    _color = WHITE;
  }

private:
  Node(const Node&);
  Node& operator= (const Node&);

};

struct Tree
{
  Node* _root;
  Node* _last;
  std::set<Node*, Node::comp> _node_set;
  std::map<size_t, Node*> _id_to_node;
  std::map<std::string, Node*> _full_path_to_node;

  explicit Tree(Node* node)
  {
    _root = node;
    _last = _root;
    _last->_parent = _root;
  }

  ~Tree()
  {
    for (std::set<Node*, Node::comp>::const_iterator it = _node_set.begin(); it != _node_set.end(); ++it)
      delete *it;
  }

  void dfs(void (* begin_func)(Node*), void (* end_func)(Node*)) const
  {
    for (std::set<Node*, Node::comp>::iterator it = _node_set.begin(); it != _node_set.end(); ++it)
      (*it)->set_white_color();
    dfs_visit(_root, begin_func, end_func);
  }

private:
  Tree(const Tree&);
  Tree& operator= (const Tree&);

  void dfs_visit(Node* node, void (* begin_func)(Node*), void (* end_func)(Node*)) const
  {
    node->_color = GRAY;
    begin_func(node);
    for(std::set<Node*, Node::comp>::iterator it = node->_child.begin(); it != node->_child.end(); ++it)
      if ((*it)->_color == WHITE)
        dfs_visit(*it, begin_func, end_func);
    node->_color = BLACK;
    end_func(node);
  }
};

//******************** BUILD **********************************************

//******** UTIL FUNCTIONS *******************

void do_nothing(Node*){}
void do_nothing(int&){}
void do_nothing(Node*, Node*){}

void get_string_number(int& string_number)
{
  std::cin >> string_number;
  std::cin.ignore();
}

void base_build_exit(bool&, bool&, bool& exit_flag,
              int string_number, int current_string_number,
              const std::string&, size_t)
{
  if (current_string_number == string_number)
    exit_flag = true;
}

void xml_base_build_exit(bool& break_flag, bool& continue_flag, bool&, int,
                         int, const std::string& temp_str, size_t first_space_number)
{
  if (temp_str == "</dir>" && first_space_number == 0)
      break_flag = true;
  else if (temp_str == "</dir>")
      continue_flag = true;
}

void base_fill_path_id(Node* node, const std::string& temp_str, size_t)
{
  size_t separate_space_index = temp_str.find(" ");
  node->_local_path = temp_str.substr(0, separate_space_index);
  node->_full_path = node->_local_path;

  std::stringstream sstr;
  sstr << temp_str.substr(separate_space_index);
  sstr >> node->_id;
}

void python_fill_path_id_deep(Node* node, const std::string& temp_str, size_t first_space_number)
{
  base_fill_path_id(node, temp_str, first_space_number);
  node->_deep = first_space_number / 4;
}

void xml_fill_path_id_deep(Node* node, const std::string& temp_str, size_t first_space_number)
{
  size_t path_begin_index = temp_str.find_first_of("'") + 1;
  size_t path_end_index = temp_str.find_first_of("'", path_begin_index) - 1;
  size_t path_length = path_end_index - path_begin_index + 1;
  node->_local_path = temp_str.substr(path_begin_index, path_length);
  node->_full_path = node->_local_path;

  size_t id_begin_index = temp_str.find_first_of("'", path_end_index + 2) + 1;
  size_t id_end_index = temp_str.find_first_of("'", id_begin_index) - 1;
  size_t id_length = id_end_index - id_begin_index + 1;
  std::stringstream sstr;
  sstr << temp_str.substr(id_begin_index, id_length);
  sstr >> node->_id;

  node->_deep = first_space_number / 2;
}

Tree* base_build(void (* get_string_number)(int&),
          void (* base_build_exit)(bool&, bool&, bool&, int, int, const std::string&, size_t),
          void (* fill_path_id_deep)(Node*, const std::string&, size_t),
          void (* not_first_iteration)(Node*, Node*))
{
  int string_number = -1;
  get_string_number(string_number);
  Tree* tree;

  bool exit_flag = false;
  bool first_itaration = true;

  int current_string_number = 0;

  while (!exit_flag)
  {
    ++current_string_number;

    bool continue_flag = false;
    bool break_flag = false;

    std::string temp_str;
    std::getline(std::cin, temp_str);
    size_t first_space_number = temp_str.find_first_not_of(" ");

    if (first_space_number > 0)
      temp_str = temp_str.substr(first_space_number);

    base_build_exit(break_flag, continue_flag, exit_flag, string_number,
            current_string_number, temp_str, first_space_number);

    if (break_flag)
      break;
    else if (continue_flag)
      continue;

    Node* new_node = new Node;

    fill_path_id_deep(new_node, temp_str, first_space_number);
    
    if (first_itaration)
    {
      tree = new Tree(new_node);
      first_itaration = false;
    }
    else
      not_first_iteration(tree->_last, new_node);

    tree->_node_set.insert(new_node);
    tree->_id_to_node[new_node->_id] = new_node;
    tree->_full_path_to_node[new_node->_full_path] = new_node;
    tree->_last = new_node;
  }
  return tree;
}

void build_python_xml_helper(Node* last, Node* new_node)
{
  if (last->_deep >= new_node->_deep)
    return build_python_xml_helper(last->_parent, new_node);
  
  new_node->_full_path = last->_full_path + "/" + new_node->_local_path;
  last->_child.insert(new_node);
  new_node->_parent = last;
}

void make_parent_child(Node* parent, Node* child)
{
  parent->_child.insert(child);
  child->_full_path = parent->_full_path + "/" + child->_local_path;
  child->_deep = parent->_deep + 1;
  child->_parent = parent;
}

//************ BUILD FUNCTIONS ****************************************

Tree* build_from_find()
{
  Tree* tree = base_build(get_string_number, base_build_exit, base_fill_path_id, do_nothing);

  for (std::set<Node*, Node::comp>::iterator it = ++tree->_node_set.begin(); it != tree->_node_set.end(); ++it)
  {
  	  std::string path = (*it)->_full_path;
  	  size_t last_separate_index = path.find_last_of("/");
  	  (*it)->_local_path = path.substr(last_separate_index + 1);

  	  std::string parent_full_path = path.substr(0, last_separate_index);
      Node* parent = tree->_full_path_to_node[parent_full_path];
      make_parent_child(parent, *it);
  }
  return tree;
}

Tree* build_from_python()
{
  return base_build(get_string_number, base_build_exit, python_fill_path_id_deep, build_python_xml_helper);
}

Tree* build_from_acm1()
{
  Tree* tree = base_build(get_string_number, base_build_exit, base_fill_path_id, do_nothing);

  for (std::set<Node*, Node::comp>::iterator it = tree->_node_set.begin(); it != tree->_node_set.end(); ++it)
  {
    size_t childdren_number = 0;
    std::cin >> childdren_number;
    
    for (size_t i = 0; i < childdren_number; ++i)
    {
      size_t child_index = 0;
      std::cin >> child_index;
      Node* temp_node = tree->_id_to_node[child_index];
      make_parent_child(*it, temp_node);
    }
  }
  return tree;
}

Tree* build_from_acm2()
{
  Tree* tree = base_build(get_string_number, base_build_exit, base_fill_path_id, do_nothing);

  for (std::set<Node*, Node::comp>::iterator it = tree->_node_set.begin(); it != tree->_node_set.end(); ++it)
  {
    int parent_index = -1;
    std::cin >> parent_index;
    if (parent_index >= 0)
    {
      Node* temp_node = tree->_id_to_node[parent_index];
      make_parent_child(temp_node, *it);
    }
  }
  return tree;
}

Tree* build_from_acm3()
{
  Tree* tree = base_build(get_string_number, base_build_exit, base_fill_path_id, do_nothing);

  size_t node_set_size = tree->_node_set.size();
  for (size_t i = 0; i < node_set_size - 1; ++i)
  {
    size_t parent_id = 0;
    size_t child_id = 0;
    std::cin >> parent_id >> child_id;
    make_parent_child(tree->_id_to_node[parent_id], tree->_id_to_node[child_id]);
  }
  return tree;
}

Tree* build_from_xml()
{
  return base_build(do_nothing, xml_base_build_exit, xml_fill_path_id_deep, build_python_xml_helper);
}

//**********************  PRINT **********************************

//******** UTIL FUNCTIONS *******************

void print_full_path_id(Node* node)
{
  std::cout << node->_full_path << " " << node->_id << std::endl;
}

void print_space(int space_number)
{
  for (int i = 0; i < space_number; ++i)
    std::cout << " ";
}

void print_python_helper(Node* node)
{
  print_space(4 * node->_deep);
  std::cout << node->_local_path << " " << node->_id << std::endl;
}

void print_id(Node* node)
{
  std::cout << " " << node->_id;
}

void print_children(Node* node)
{
  std::cout << node->_child.size();
  for_each(node->_child.begin(), node->_child.end(), print_id);
  std::cout << std::endl;
}

void print_local_path(Node* node)
{
  std::cout << node->_local_path << " " << node->_id << std::endl;
}

void print_parent_id(Node* node)
{
  std::cout << node->_parent->_id << std::endl;
}

void print_parent_children(Node* node)
{
  for (std::set<Node*, Node::comp>::iterator it = node->_child.begin(); it != node->_child.end(); ++it)
    std::cout << node->_id << " " << (*it)->_id << std::endl;
}

void print_path_and_id_in_xml(Node* node)
{
  print_space(2 * node->_deep);
  std::cout << "<";
  if (node->_child.size() > 0)
    std::cout << "dir name='" << node->_local_path << "' id='" << node->_id << "'>" << std::endl;
  else
    std::cout << "file name='" << node->_local_path << "' id='" << node->_id << "'/>" << std::endl;
}

void close_tag(Node* node)
{
  if (node->_child.size() > 0)
  {
    print_space(2 * node->_deep);
    std::cout << "</dir>" << std::endl;
  }
}

//******** PRINT FUNCTIONS *******************

void print_in_find(const Tree& tree)
{
  std::cout << tree._node_set.size() << std::endl;
  for_each(tree._node_set.begin(), tree._node_set.end(), print_full_path_id);
}

void print_in_python(const Tree& tree)
{
  std::cout << tree._node_set.size() << std::endl;
  tree.dfs(print_python_helper, do_nothing);
}

void print_in_acm1(const Tree& tree)
{
  std::cout << tree._node_set.size() << std::endl;
  for_each(tree._node_set.begin(), tree._node_set.end(), print_local_path);
  for_each(tree._node_set.begin(), tree._node_set.end(), print_children);
}

void print_in_acm2(const Tree& tree)
{
  std::cout << tree._node_set.size() << std::endl;
  for_each(tree._node_set.begin(), tree._node_set.end(), print_local_path);
  std::cout << -1 << std::endl;
  for_each(++tree._node_set.begin(), tree._node_set.end(), print_parent_id);
}

void print_in_acm3(const Tree& tree)
{
  std::cout << tree._node_set.size() << std::endl;
  for_each(tree._node_set.begin(), tree._node_set.end(), print_local_path);
  for_each(tree._node_set.begin(), tree._node_set.end(), print_parent_children);
}

void print_in_xml(const Tree& tree)
{
  tree.dfs(print_path_and_id_in_xml, close_tag);
}


typedef Tree* (* pbuild_func)();
typedef void (* pprint_func)(const Tree& tree);


int main()
{
  std::map<std::string, pbuild_func> build_map;
  build_map["find"] = & build_from_find;
  build_map["python"] = & build_from_python;
  build_map["acm1"] = & build_from_acm1;
  build_map["acm2"] = & build_from_acm2;
  build_map["acm3"] = & build_from_acm3;
  build_map["xml"] = & build_from_xml;

  std::map<std::string, pprint_func> print_map;
  print_map["find"] = & print_in_find;
  print_map["python"] = & print_in_python;
  print_map["acm1"] = & print_in_acm1;
  print_map["acm2"] = & print_in_acm2;
  print_map["acm3"] = & print_in_acm3;
  print_map["xml"] = & print_in_xml;

  std::string input_format;
  std::string output_format;

  getline(std::cin, input_format);
  getline(std::cin, output_format);

  Tree* tree = build_map[input_format]();
  print_map[output_format](*tree);

  return 0;
}