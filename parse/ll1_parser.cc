#include <cassert>
#include <cctype>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Node {
 public:
  Node() = default;
  Node(const std::string& name) {
    if (name.front() == '<' && name.back() == '>' && name.size() > 2) {
      name_ = name.substr(1, name.size() - 2);
      terminal_ = false;
    } else {
      name_ = name;
      terminal_ = true;
    }
  }

  const std::string& GetName() const { return name_; }

  std::string Repr() const {
    if (terminal_) return name_;
    return "<" + name_ + ">";
  }

  bool IsTerminal() const { return terminal_; }

  bool IsNonTerminal() const { return !terminal_; }

  static bool IsTerminal(const std::string& name) {
    if (name.front() == '<' && name.back() == '>' && name.size() > 2) {
      return false;
    }
    return true;
  }

  static bool IsNonTerminal(const std::string& name) { return !IsTerminal(name); }

 private:
  std::string name_;
  bool terminal_ = false;
};

// parent : child1
//        | child2
//        | child3
class Grammar {
 public:
  Grammar(const std::string& g) {
    auto size = g.size();
    auto i = 0ULL;

    // skip whitespace
    for (; i < size; ++i) {
      if (!std::isspace(g[i])) break;
    }

    // parent
    std::string name;
    for (; i < size; ++i) {
      if (std::isspace(g[i])) break;
      if (g[i] == ':' || g[i] == '|') break;
      name += g[i];
    }
    if (name.empty()) goto error;

    // skip whitespace
    for (; i < size; ++i) {
      if (!std::isspace(g[i])) break;
    }

    if (i > size - 2) goto error;
    if (g[i++] != ':') goto error;

    {
      std::vector<std::vector<Node>> rules;
      std::vector<Node> tmp_rule;
      for (; i < size; ++i) {
        if (std::isspace(g[i])) continue;

        if (g[i] == '|') {
          if (tmp_rule.empty()) goto error;
          rules.push_back(std::move(tmp_rule));
          tmp_rule.clear();
        } else {
          auto j = 0ULL;
          for (; i < size && !std::isspace(g[i]) && g[i] != '|'; ++i) {
            if (g[i] == ':') goto error;
            ++j;
          }
          tmp_rule.emplace_back(g.substr(i - j, j));
          --i;
        }
      }

      if (tmp_rule.empty()) goto error;

      rules.push_back(std::move(tmp_rule));
      parent_ = Node(name);
      children_ = std::move(rules);
    }

    return;

  error:
    std::cerr << "'" << g << "' syntax error\n";
  }

  void Display() const {
    std::string buf = parent_.Repr() + " : ";
    std::string prefix(buf.size() - 2, ' ');
    assert(children_.size() > 0);
    for (const auto& node : children_[0]) {
      buf += node.Repr() + " ";
    }
    buf += "\n";
    for (auto i = 1ULL; i < children_.size(); ++i) {
      buf += prefix + "| ";
      for (const auto& node : children_[i]) {
        buf += node.Repr() + " ";
      }
      buf += "\n";
    }

    std::cout << buf;
  }

  const Node& GetParent() const { return parent_; }
  const std::vector<std::vector<Node>>& GetChildren() const { return children_; }

 private:
  Node parent_;
  std::vector<std::vector<Node>> children_;
};

class CFG {
 public:
  static constexpr const char* EPSILON = "epsilon";

  void AddGrammar(const std::string& g) { productions_.emplace_back(g); }

  std::string GetStartSymbol() const {
    if (productions_.empty()) return "";
    std::unordered_set<std::string> set;
    for (const auto& g : productions_) {
      const auto& parent = g.GetParent();
      set.insert(parent.Repr());
    }

    for (const auto& g : productions_) {
      const auto& children = g.GetChildren();
      for (const auto& child : children) {
        for (const auto& node : child)
          if (node.IsNonTerminal()) set.erase(node.Repr());
      }
    }

    assert(set.size() == 1);
    return *set.begin();
  }

  void Display() const {
    for (const auto& p : productions_) {
      p.Display();
      std::cout << "\n";
    }
  }

  void Parse(const std::vector<std::string>& words) {
    CreateFirstSet();
    std::cout << "======================================================\n";
    CreateFollowSet();
    std::cout << "======================================================\n";
    CreateAugmentedFirstSet();
    std::cout << "======================================================\n";
    CreateTable();
    std::cout << "======================================================\n";

    std::vector<std::string> stack;
    stack.push_back("eof");
    stack.push_back(GetStartSymbol());
    auto word = words.cbegin();
    while (true) {
      std::cout << "[debug]";
      for (const auto& node : stack) {
        std::cout << " " << node;
      }
      std::cout << "\n";
      if (stack.back() == "eof" && *word == "eof") {
        std::cout << "Sucecess!\n";
        break;
      }
      if (Node::IsTerminal(stack.back()) || stack.back() == "eof") {
        if (stack.back() == *word) {
          stack.pop_back();
          ++word;
        } else {
          std::cout << "an error looking for symbol at top of stack\n";
          break;
        }
      } else {
        if (table_.count(stack.back() + " - " + *word)) {
          const auto& p = table_[stack.back() + " - " + *word];
          auto pivot = p.find('-');
          auto x = std::stoull(p.substr(0, pivot));
          auto y = std::stoull(p.substr(pivot + 1));
          const auto& child = productions_[x].GetChildren()[y];
          stack.pop_back();
          for (auto i = child.size(); i > 0; --i) {
            if (child[i - 1].Repr() != EPSILON) stack.push_back(child[i - 1].Repr());
          }
        } else {
          std::cout << "an error expanding focus\n";
          break;
        }
      }
    }
  }

 private:
  using SetMap = std::unordered_map<std::string, std::unordered_set<std::string>>;
  using Table = std::unordered_map<std::string, std::string>;

  void CreateFirstSet() {
    SetMap first_set;
    for (const auto& g : productions_) {
      const auto& children = g.GetChildren();
      for (const auto& child : children) {
        for (const auto& node : child)
          if (node.IsTerminal()) first_set[node.Repr()] = {node.Repr()};
      }
    }

    bool change = true;
    while (change) {
      change = false;
      for (const auto& g : productions_) {
        const auto& p = g.GetParent();
        const auto& children = g.GetChildren();
        for (const auto& child : children) {
          std::unordered_set<std::string> rhs;
          auto key = child[0].Repr();
          if (child[0].IsTerminal()) {
            rhs.insert(key);
          } else {
            rhs = first_set[key];
          }
          rhs.erase(EPSILON);

          auto i = 0ULL;
          auto size = child.size();
          while (first_set[child[i].Repr()].count(EPSILON) && i < size - 1) {
            auto tmp = first_set[child[i + 1].Repr()];
            tmp.erase(EPSILON);
            rhs.merge(std::move(tmp));
            ++i;
          }

          if (i == size - 1 and first_set[child[i].Repr()].count(EPSILON)) rhs.insert(EPSILON);
          auto n = first_set[p.Repr()].size();
          first_set[p.Repr()].merge(std::move(rhs));
          if (n != first_set[p.Repr()].size()) change = true;
        }
      }
    }

    for (const auto& i : first_set) {
      std::cout << i.first << " : ";
      for (const auto& j : i.second) {
        std::cout << j << ", ";
      }
      std::cout << "\n";
    }

    first_set_ = std::move(first_set);
  }

  void CreateFollowSet() {
    SetMap follow_set;
    const auto& start_symbol = GetStartSymbol();
    std::cout << "[start symbol] " << start_symbol << "\n";
    follow_set[start_symbol] = {"eof"};

    bool change = true;
    while (change) {
      change = false;
      for (const auto& g : productions_) {
        const auto& p = g.GetParent();
        const auto& children = g.GetChildren();
        for (const auto& child : children) {
          auto trailer = follow_set[p.Repr()];
          for (auto i = child.size(); i > 0; --i) {
            if (child[i - 1].IsNonTerminal()) {
              auto n = follow_set[child[i - 1].Repr()].size();
              follow_set[child[i - 1].Repr()].merge(trailer);
              if (n != follow_set[child[i - 1].Repr()].size()) change = true;
              if (first_set_[child[i - 1].Repr()].count(EPSILON)) {
                auto tmp = first_set_[child[i - 1].Repr()];
                tmp.erase(EPSILON);
                trailer.merge(std::move(tmp));
              } else {
                trailer = first_set_[child[i - 1].Repr()];
              }
            } else {
              trailer = first_set_[child[i - 1].Repr()];
            }
          }
        }
      }
    }

    for (const auto& i : follow_set) {
      std::cout << i.first << " : ";
      for (const auto& j : i.second) {
        std::cout << j << ", ";
      }
      std::cout << "\n";
    }

    follow_set_ = std::move(follow_set);
  }

  void CreateAugmentedFirstSet() {
    SetMap augmented_first_set;
    for (auto i = 0ULL; i < productions_.size(); ++i) {
      for (auto j = 0ULL; j < productions_[i].GetChildren().size(); ++j) {
        std::string p = std::to_string(i) + "-" + std::to_string(j);
        augmented_first_set[p] = first_set_[productions_[i].GetChildren()[j].front().Repr()];
        if (first_set_[productions_[i].GetChildren()[j].front().Repr()].count(EPSILON)) {
          augmented_first_set[p].merge(follow_set_[productions_[i].GetParent().Repr()]);
        }
      }
    }

    for (const auto& i : augmented_first_set) {
      auto pivot = i.first.find('-');
      auto x = std::stoull(i.first.substr(0, pivot));
      auto y = std::stoull(i.first.substr(pivot + 1));
      std::cout << productions_[x].GetParent().Repr() << " :";
      for (const auto& node : productions_[x].GetChildren()[y]) {
        std::cout << " " << node.Repr();
      }
      std::cout << "\n\t-> ";
      for (const auto& j : i.second) {
        std::cout << j << ", ";
      }
      std::cout << "\n\n";
    }

    augmented_first_set_ = std::move(augmented_first_set);
  }

  void CreateTable() {
    Table table;
    for (auto i = 0ULL; i < productions_.size(); ++i) {
      const auto& parent = productions_[i].GetParent();
      for (auto j = 0ULL; j < productions_[i].GetChildren().size(); ++j) {
        std::string product = std::to_string(i) + "-" + std::to_string(j);
        for (const auto& w : augmented_first_set_[product]) {
          if (Node::IsNonTerminal(w)) continue;
          table[parent.Repr() + " - " + w] = product;
          if (augmented_first_set_[product].count("eof")) {
            table[parent.Repr() + " - eof"] = product;
          }
        }
      }
    }

    for (const auto& i : table) {
      std::cout << i.first << " : " << i.second << "\n";
    }
    table_ = std::move(table);
  }

  std::vector<Grammar> productions_;
  SetMap first_set_;
  SetMap follow_set_;
  SetMap augmented_first_set_;
  Table table_;
};

int main() {
  CFG cfg;
  cfg.AddGrammar("<Goal> : <Expr>");
  cfg.AddGrammar("<Expr> : <Term> <Expr'>");
  cfg.AddGrammar("<Expr'> : + <Term> <Expr'> | - <Term> <Expr'> | epsilon");
  cfg.AddGrammar("<Term> : <Factor> <Term'>");
  cfg.AddGrammar("<Term'> : * <Factor> <Term'> | /  <Factor> <Term'> | epsilon");
  cfg.AddGrammar("<Factor> : ( <Expr> ) | num | name");

  cfg.Display();
  std::cout << "======================================================\n";

  std::vector<std::string> words{"(", "name", "+", "name", ")", "*", "name", "eof"};
  cfg.Parse(words);
}
