#include <cctype>
#include <iostream>
#include <string>
#include <vector>

class NonTerminal {
 public:
  NonTerminal(const std::string& name) : name_(name) {}
  NonTerminal(std::string&& name) : name_(std::move(name)) {}

  const std::string& GetName() const { return name_; }
  std::string GetName() { return name_; }

  const std::vector<std::string>& GetRules() const { return production_rules_; }
  std::vector<std::string> GetRules() { return production_rules_; }

  void SetRules(const std::vector<std::string>& rules) { production_rules_ = rules; }
  void SetRules(std::vector<std::string>&& rules) { production_rules_ = std::move(rules); }

  void AddRule(const std::string& rule) { production_rules_.push_back(rule); }
  void AddRule(std::string&& rule) { production_rules_.push_back(std::move(rule)); }

  void PrintRules() const {
    std::string buf;
    buf += name_ + " ->";
    for (const auto& rule : production_rules_) {
      buf += " " + rule + " |";
    }
    buf.pop_back();
    buf.pop_back();
    std::cout << buf << "\n";
  }

 private:
  std::string name_;
  std::vector<std::string> production_rules_;
};

class Grammar {
 public:
  void AddRule(const std::string& rule) {
    auto size = rule.size();
    std::string name;
    auto i = 0ULL;

    // skip whitespaces
    for (; i < size; ++i)
      if (!std::isspace(rule[i])) break;

    for (; i < size; ++i) {
      if (std::isspace(rule[i])) break;
      name += rule[i];
    }
    if (name.empty()) goto error;

    // skip whitespaces
    for (; i < size; ++i)
      if (!std::isspace(rule[i])) break;

    if (i > size - 2) goto error;
    if (rule[i] != '-' && rule[i + 1] != '>') goto error;
    i += 2;

    {
      std::vector<std::string> productions;
      NonTerminal node(std::move(name));
      std::string tmp;
      for (; i < size; ++i) {
        if (std::isspace(rule[i])) continue;

        if (rule[i] != '|') {
          for (; i < size && !std::isspace(rule[i]) && rule[i] != '|'; ++i) {
            tmp += rule[i];
          }
          tmp += ' ';
          --i;
        } else {
          if (tmp.empty()) goto error;
          tmp.pop_back();
          productions.push_back(std::move(tmp));
          tmp.clear();
        }
      }

      if (tmp.empty()) goto error;
      tmp.pop_back();
      productions.push_back(std::move(tmp));
      node.SetRules(std::move(productions));
      nonterminals_.push_back(std::move(node));
    }
    return;

  error:
    std::cerr << "[error] '" << rule << "' syntax error\n";
  }

  void PrintRules() const {
    for (const auto& node : nonterminals_) {
      node.PrintRules();
    }
  }

  void EliminateLeftRecursion() {
    auto size = nonterminals_.size();
    for (auto i = 0ULL; i < size; ++i) {
      for (auto j = 0ULL; j < i; ++j) {
        SolveIndirectLeftRecursion(nonterminals_[i], nonterminals_[j]);
      }
      SolveDirectLeftRecursion(nonterminals_[i]);
    }
  }

  void SolveIndirectLeftRecursion(NonTerminal& aa, const NonTerminal& b) {
    const auto& a = aa;
    const auto& name_b = b.GetName();
    const auto& rules_a = a.GetRules();
    const auto& rules_b = b.GetRules();
    std::vector<std::string> new_rules;

    for (const auto& rule : rules_a) {
      auto i = 0ULL;
      for (; i < rule.size(); ++i)
        if (!std::isspace(rule[i])) break;

      if (rule.substr(i, name_b.size()) == name_b) {
        for (const auto& rule1 : rules_b) {
          new_rules.push_back(rule1 + rule.substr(i + name_b.size()));
        }
      } else {
        new_rules.push_back(rule);
      }
    }
    aa.SetRules(new_rules);
  }

  void SolveDirectLeftRecursion(NonTerminal& node) {
    const auto& name = const_cast<const NonTerminal&>(node).GetName();
    const auto& rules = const_cast<const NonTerminal&>(node).GetRules();
    auto new_name = name + "'";
    std::vector<std::string> alphas, betas, new_rules, new_rules1;

    for (const auto& rule : rules) {
      auto i = 0ULL;
      for (; i < rule.size(); ++i)
        if (!std::isspace(rule[i])) break;

      if (rule.substr(i, name.size()) == name) {
        alphas.push_back(rule.substr(i + name.size()));
      } else {
        betas.push_back(rule);
      }
    }

    if (alphas.empty()) return;
    if (betas.empty()) new_rules.push_back(new_name);
    for (const auto& beta : betas) new_rules.push_back(beta + " " + new_name);
    for (const auto& alpha : alphas) new_rules1.push_back(alpha + " " + new_name);

    node.SetRules(new_rules);
    new_rules1.push_back("epsilon");

    NonTerminal new_node(new_name);
    new_node.SetRules(new_rules1);
    nonterminals_.push_back(std::move(new_node));
  }

 private:
  std::vector<NonTerminal> nonterminals_;
};

int main() {
  Grammar g;
  g.AddRule("A1 -> A2 A3");
  g.AddRule("A2 -> A3 A1 | b");
  g.AddRule("A3 -> A1 A1 | a");

  g.PrintRules();
  std::cout << "====================\n";
  g.EliminateLeftRecursion();
  g.PrintRules();

  return 0;
}
