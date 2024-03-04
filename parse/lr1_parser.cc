#include <cassert>
#include <cctype>
#include <functional>
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

template <typename T>
void HashCombine(std::size_t& seed, const T& val) {
  seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct Lr1Item {
  std::size_t index_i;
  std::size_t index_j;
  std::size_t position;
  std::string lookahead_symbol;

  bool operator==(const Lr1Item& other) const {
    return index_i == other.index_i && index_j == other.index_j && position == other.position &&
           lookahead_symbol == other.lookahead_symbol;
  }
};

template <>
struct std::hash<Lr1Item> {
  std::size_t operator()(const Lr1Item& key) const {
    auto hash1 = std::hash<std::size_t>()(key.index_i);
    auto hash2 = std::hash<std::size_t>()(key.index_j);
    auto hash3 = std::hash<std::size_t>()(key.position);
    auto hash4 = std::hash<std::string>()(key.lookahead_symbol);
    HashCombine(hash1, hash2);
    HashCombine(hash1, hash3);
    HashCombine(hash1, hash4);
    return hash1;
  }
};

struct GotoKey {
  std::size_t index;
  std::string symbol;

  bool operator==(const GotoKey& other) const {
    return index == other.index && symbol == other.symbol;
  }
};

template <>
struct std::hash<GotoKey> {
  std::size_t operator()(const GotoKey& key) const {
    auto hash1 = std::hash<std::size_t>()(key.index);
    auto hash2 = std::hash<std::string>()(key.symbol);
    HashCombine(hash1, hash2);
    return hash1;
  }
};

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
      production_head_ = Node(name);
      productions_ = std::move(rules);
    }

    return;

  error:
    std::cerr << "'" << g << "' syntax error\n";
  }

  void Display() const {
    std::string buf = production_head_.Repr() + " : ";
    std::string prefix(buf.size() - 2, ' ');
    assert(productions_.size() > 0);
    for (const auto& node : productions_[0]) {
      buf += node.Repr() + " ";
    }
    buf += "\n";
    for (auto i = 1ULL; i < productions_.size(); ++i) {
      buf += prefix + "| ";
      for (const auto& node : productions_[i]) {
        buf += node.Repr() + " ";
      }
      buf += "\n";
    }

    std::cout << buf;
  }

  const Node& GetProductionHead() const { return production_head_; }
  const std::vector<std::vector<Node>>& GetProductions() const { return productions_; }

 private:
  Node production_head_;
  std::vector<std::vector<Node>> productions_;
};

class CFG {
 public:
  static constexpr const char* EPSILON = "epsilon";

  void AddGrammar(const std::string& g) { grammar_rules_.emplace_back(g); }

  std::string GetStartSymbol() const {
    if (grammar_rules_.empty()) return "";
    std::unordered_set<std::string> set;
    for (const auto& g : grammar_rules_) {
      const auto& parent = g.GetProductionHead();
      set.insert(parent.Repr());
    }

    for (const auto& g : grammar_rules_) {
      const auto& children = g.GetProductions();
      for (const auto& child : children) {
        for (const auto& node : child)
          if (node.IsNonTerminal()) set.erase(node.Repr());
      }
    }

    assert(set.size() == 1);
    return *set.begin();
  }

  void Display() const {
    for (const auto& p : grammar_rules_) {
      p.Display();
      std::cout << "\n";
    }
  }

  void Parse(const std::vector<std::string>& words) {
    CreateFirstSet();
    BuildCc();
    TableFill();
    std::cout << "======================================================\n";
    auto i = 0;
    std::stack<std::variant<std::size_t, std::string>> stack;
    stack.push("<Goal>");
    stack.push((std::size_t)0);
    while (true) {
      auto state = stack.top();
      if (!action_table_.count({std::get<std::size_t>(state), words[i]})) {
        std::cout << "fail\n";
        return;
      }
      const auto& action = action_table_[{std::get<std::size_t>(state), words[i]}];
      if (action.first == "reduce") {
        const auto& ij = action.second;
        auto i = std::stoull(ij.substr(0, ij.find_first_of("-")));
        auto j = std::stoull(ij.substr(ij.find_first_of(">") + 1));
        std::cout << "debug reduce " << i << ", " << j << std::endl;
        const auto& production_head = grammar_rules_[i].GetProductionHead();
        const auto& productions = grammar_rules_[i].GetProductions();
        const auto& production = productions[j];
        for (auto k = 0ULL; k < production.size(); ++k) {
          stack.pop();
          stack.pop();
        }
        state = stack.top();
        stack.push(production_head.Repr());
        stack.push(
            std::stoull(goto_table_[{std::get<std::size_t>(state), production_head.Repr()}].first));
      } else if (action.first == "shift") {
        std::cout << "debug shift " << words[i] << ", " << (std::size_t)std::stoull(action.second)
                  << std::endl;
        stack.push(words[i]);
        stack.push((std::size_t)std::stoull(action.second));
        ++i;
      } else if (action.first == "accept") {
        break;
      } else {
        std::cout << "fail\n";
        return;
      }
    }

    std::cout << "success\n";
  }

 private:
  using CanonicalCollection = std::unordered_set<Lr1Item>;
  using CanonicalCollectionSet = std::vector<CanonicalCollection>;
  using Set = std::unordered_set<std::string>;
  using SetMap = std::unordered_map<std::string, Set>;

  void CreateFirstSet() {
    SetMap first_set;
    for (const auto& g : grammar_rules_) {
      const auto& children = g.GetProductions();
      for (const auto& child : children) {
        for (const auto& node : child)
          if (node.IsTerminal()) first_set[node.Repr()] = {node.Repr()};
      }
    }

    bool change = true;
    while (change) {
      change = false;
      for (const auto& g : grammar_rules_) {
        const auto& p = g.GetProductionHead();
        const auto& children = g.GetProductions();
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

  Set FirstSet(const Lr1Item& item) {
    Set first_set;
    const auto& productions = grammar_rules_[item.index_i].GetProductions();
    const auto& production = productions[item.index_j];
    for (auto i = item.position + 1; i < production.size(); ++i) {
      const auto& node = production[i];
      for (const auto& j : first_set_[node.Repr()]) first_set.insert(j);
      if (node.IsTerminal()) break;
      if (first_set_[node.Repr()].count(EPSILON) == 0) break;
    }
    if (first_set.count(EPSILON)) first_set.insert(item.lookahead_symbol);
    if (item.position + 1 >= production.size()) first_set.insert(item.lookahead_symbol);
    return first_set;
  }

  void Closure(CanonicalCollection& set) {
    bool change = true;
    while (change) {
      change = false;
      for (const auto& item : set) {
        const auto& productions = grammar_rules_[item.index_i].GetProductions();
        const auto& production = productions[item.index_j];
        if (item.position >= production.size()) continue;
        if (production[item.position].IsTerminal()) continue;
        auto first_set = FirstSet(item);
        for (auto i = 0ULL; i < grammar_rules_.size(); ++i) {
          const auto& g = grammar_rules_[i];
          if (g.GetProductionHead().Repr() == production[item.position].Repr()) {
            for (auto j = 0ULL; j < g.GetProductions().size(); ++j) {
              for (const auto& b : first_set) {
                auto old_size = set.size();
                set.insert({i, j, 0, b});
                auto new_size = set.size();
                if (old_size != new_size) {
                  change = true;
                }
              }
            }
          }
        }
      }
    }
  }

  CanonicalCollection Goto(const CanonicalCollection& s, const std::string& x) {
    CanonicalCollection moved;
    for (const auto& item : s) {
      const auto& productions = grammar_rules_[item.index_i].GetProductions();
      const auto& production = productions[item.index_j];
      if (item.position >= production.size()) continue;
      if (production[item.position].Repr() != x) continue;
      auto tmp_item = item;
      ++tmp_item.position;
      moved.insert(std::move(tmp_item));
    }
    Closure(moved);
    return moved;
  }

  void BuildCc() {
    CanonicalCollection cc0;
    cc0.insert({0, 0, 0, "eof"});
    Closure(cc0);
    cc_set_.push_back(std::move(cc0));

    for (auto i = 0ULL; i < cc_set_.size(); ++i) {
      for (const auto& item : cc_set_[i]) {
        const auto& productions = grammar_rules_[item.index_i].GetProductions();
        const auto& production = productions[item.index_j];
        if (item.position >= production.size()) continue;
        auto tmp = Goto(cc_set_[i], production[item.position].Repr());
        bool include = false;
        auto index = 0ULL;
        for (auto j = 0ULL; j < cc_set_.size(); ++j) {
          const auto& tmp_cc = cc_set_[j];
          if (tmp.size() == tmp_cc.size()) {
            bool same = true;
            for (const auto& item1 : tmp) {
              if (!tmp_cc.count(item1)) {
                same = false;
                break;
              }
            }
            if (same) {
              include = true;
              index = j;
              break;
            }
          }
        }
        if (!include) {
          cc_set_.push_back(std::move(tmp));
          goto_[{i, production[item.position].Repr()}] = cc_set_.size() - 1;
        } else {
          goto_[{i, production[item.position].Repr()}] = index;
        }
      }
    }
    for (const auto& set : cc_set_) {
      std::cout << "CC item ======================================================\n";
      for (const auto& item : set) {
        const auto& production_head = grammar_rules_[item.index_i].GetProductionHead();
        const auto& productions = grammar_rules_[item.index_i].GetProductions();
        const auto& production = productions[item.index_j];
        std::cout << "[" << production_head.Repr() << " -> ";
        for (auto i = 0ULL; i < production.size(); ++i) {
          if (i == item.position) std::cout << "@ ";
          const auto& g = production[i];
          std::cout << g.Repr() << " ";
        }
        if (item.position >= production.size()) std::cout << "@ ";
        std::cout << ", " << item.lookahead_symbol << "]\n";
      }
    }
  }

  void TableFill() {
    for (auto i = 0ULL; i < cc_set_.size(); ++i) {
      const auto& cc_i = cc_set_[i];
      for (const auto& item : cc_i) {
        // const auto& production_head = grammar_rules_[item.index_i].GetProductionHead();
        const auto& productions = grammar_rules_[item.index_i].GetProductions();
        const auto& production = productions[item.index_j];

        if (item.index_i == 0 && item.index_j == 0 && item.position == 1 &&
            item.lookahead_symbol == "eof") {
          action_table_[{i, "eof"}] = {"accept", ""};
          continue;
        }

        if (item.position >= production.size()) {
          action_table_[{i, item.lookahead_symbol}] = {
              "reduce", std::to_string(item.index_i) + "->" + std::to_string(item.index_j)};
          continue;
        }

        GotoKey tmp_key = {i, production[item.position].Repr()};
        if (production[item.position].IsNonTerminal()) continue;
        if (!goto_.count(tmp_key)) continue;
        action_table_[{i, production[item.position].Repr()}] = {"shift",
                                                                std::to_string(goto_[tmp_key])};
      }

      for (auto j = 0ULL; j < grammar_rules_.size(); ++j) {
        const auto& production_head = grammar_rules_[j].GetProductionHead();
        GotoKey key = {i, production_head.Repr()};
        if (goto_.count(key)) {
          goto_table_[{i, production_head.Repr()}] = {std::to_string(goto_[key]), ""};
        }
      }
    }

    std::cout << "Action table ======================================================\n";
    for (const auto& item : action_table_) {
      std::cout << item.first.index << ", " << item.first.symbol << ", " << item.second.first
                << ", " << item.second.second << "\n";
    }

    std::cout << "Goto table ======================================================\n";
    for (const auto& item : goto_table_) {
      std::cout << item.first.index << ", " << item.first.symbol << ", " << item.second.first
                << ", " << item.second.second << "\n";
    }
  }

  std::vector<Grammar> grammar_rules_;
  SetMap first_set_;
  CanonicalCollectionSet cc_set_;
  std::unordered_map<GotoKey, std::size_t> goto_;
  std::unordered_map<GotoKey, std::pair<std::string, std::string>> action_table_;
  std::unordered_map<GotoKey, std::pair<std::string, std::string>> goto_table_;
};

int main() {
  CFG cfg;
  cfg.AddGrammar("<Goal> : <List>");
  cfg.AddGrammar("<List> : <List> <Pair> | <Pair>");
  cfg.AddGrammar("<Pair> : ( <Pair> ) | ( )");
  // cfg.AddGrammar("<Goal> : <Stmt>");
  // cfg.AddGrammar("<Stmt> : if expr then <Stmt> | if expr then <Stmt> else <Stmt> | assign");

  cfg.Display();
  std::cout << "======================================================\n";

  std::vector<std::string> words{"(", "(", ")", ")", "(", ")", "eof"};
  cfg.Parse(words);
}
