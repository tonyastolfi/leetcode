#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <assert.h>

using namespace std;

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode(int x) : val(x), left(NULL), right(NULL) {}
  TreeNode(int x, TreeNode *l, TreeNode *r) : val(x), left(l), right(r) {}
};

template <typename T> ostream &operator<<(ostream &out, const vector<T> &v) {
  out << "[";
  for (auto &i : v) {
    out << i << ",";
  }
  return out << "]";
}

ostream &operator<<(ostream &out, const TreeNode *node) {
  if (!node) {
    return out;
  }
  return out << "<" << node->left << node->val << node->right << ">";
}

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution {
public:
  struct TreeEquals {
    bool operator()(const TreeNode &lhs, const TreeNode &rhs) const {
      return lhs.val == rhs.val && lhs.left == rhs.left &&
             lhs.right == rhs.right;
    }
  };

  struct TreeHash {
    size_t operator()(const TreeNode &node) const {
      const size_t left_hash = std::hash<TreeNode *>()(node.left);
      const size_t right_hash = std::hash<TreeNode *>()(node.right);
      return munge(
          munge(munge(munge(0x915678327523876) ^ node.val) ^ left_hash) ^
          right_hash);
    }

    static size_t munge(const size_t val) {
      return (val << 17) ^ (val << 5) ^ (val >> 23);
    }
  };

  template <typename Pre, typename In, typename Post>
  void traverse(TreeNode *root, Pre &&pre_action, In &&in_action,
                Post &&post_action) {
    vector<TreeNode *> stack;
    TreeNode *next = root;
    for (;;) {
      // enter next
      do {
        pre_action(next);
        stack.push_back(next);
        if (!next->left) {
          if (next->right) {
            // visit inner node with no left subtree
            in_action(next);
          }
          next = next->right;
        } else {
          next = next->left;
        }
      } while (next);

      // visit leaf
      in_action(stack.back());

      // advance
      for (;;) {
        TreeNode *const prev = stack.back();
        stack.pop_back();
        post_action(prev);
        if (stack.empty()) {
          return;
        }
        if (!stack.back()->right) {
          // visit inner node with no right subtree
          in_action(stack.back());
          continue;
        }
        if (prev == stack.back()->left) {
          break;
        }
      }

      // visit inner node with left and right subtrees
      in_action(stack.back());

      // process the right subtree
      next = stack.back()->right;
    }
  }

  vector<TreeNode *> findDuplicateSubtrees(TreeNode *root) {
    if (!root)
      return {};
    unordered_map<TreeNode *, TreeNode *> first_by_ref;
    unordered_map<TreeNode, TreeNode *, TreeHash, TreeEquals> first_by_val;
    unordered_set<TreeNode *> duplicates;
    traverse(root,
             // pre-action
             [](TreeNode *) {},
             // in-action
             [](TreeNode *n) { cout << "  " << n << endl; },
             // post-action
             [&](TreeNode *node) {
               TreeNode val(node->val);
               val.left = node->left ? first_by_ref[node->left] : nullptr;
               val.right = node->right ? first_by_ref[node->right] : nullptr;
               auto iter = first_by_val.find(val);
               if (iter != first_by_val.end()) {
                 first_by_ref[node] = iter->second;
                 duplicates.insert(iter->second);
               } else {
                 first_by_val[val] = node;
                 first_by_ref[node] = node;
               }
             });
    return vector<TreeNode *>(duplicates.begin(), duplicates.end());
  }
};

int main() {
  Solution s;
  auto *input = new TreeNode(0,                            //
                             new TreeNode(0,               //
                                          new TreeNode(0), //
                                          nullptr),        //
                             new TreeNode(0,               //
                                          nullptr,
                                          new TreeNode(0,               //
                                                       new TreeNode(0), //
                                                       nullptr)));

  auto ans = s.findDuplicateSubtrees(input);
  cout << input << endl;
  cout << ans << endl;
  return 0;
}
