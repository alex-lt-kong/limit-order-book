#ifndef BST_LOCKED_IMPL_H
#define BST_LOCKED_IMPL_H

#include <functional>
#include <print>
#include <vector>

namespace OrderBookProgrammingProblem {
  template<std::totally_ordered T>
  struct TreeNode {
    T val;
    TreeNode *left;
    TreeNode *right;

    TreeNode() : val(0), left(nullptr), right(nullptr) {
    }

    explicit TreeNode(T x) : val(x), left(nullptr), right(nullptr) {
    }
  };

  // Enum for traversal order
  enum class TraversalOrder { LeftRootRight, RightRootLeft };


  template<std::totally_ordered T>
  class BinarySearchTree {
  public:
    using CallbackType = std::function<bool(void *, T &)>;
    // insert() method passed all tests at
    // https://leetcode.com/problems/insert-into-a-binary-search-tree/description/
    static bool insert(TreeNode<T> **root, T val) {
      if (*root == nullptr) {
        *root = new TreeNode<T>(val);
        return true;
      }
      if ((*root)->val > val) {
        return insert(&((*root)->left), val);
      }
      if ((*root)->val < val) {
        return insert(&((*root)->right), val);
      }
      // ((*root)->val == val)
      return false;
    }

    // inorder_traversal() method passed all tests at
    // https://leetcode.com/problems/binary-tree-inorder-traversal/description/
    // Note that this problem is about the inorder traversal of a binary
    // tree, not limited to a BST
    static std::vector<T> inorder_traversal(TreeNode<T> *root) {
      if (root == nullptr) {
        return {};
      }
      std::vector<T> result;
      if (root->left != nullptr) {
        auto left_result = inorder_traversal(root->left);
        result.insert(result.end(), left_result.begin(), left_result.end());
      }
      result.push_back(root->val);
      // std::print("{:>6}, ", root->val);
      if (root->right != nullptr) {
        auto right_result = inorder_traversal(root->right);
        result.insert(result.end(), right_result.begin(), right_result.end());
      }
      return result;
    }

    template<TraversalOrder Order>
    static bool inorder_traversal_cb(TreeNode<T> *root,
                                     const CallbackType &callback,
                                     void *context) {
      if (root == nullptr)
        return true;

      if constexpr (Order != TraversalOrder::LeftRootRight) {
        if (root->left != nullptr)
          if (!inorder_traversal_cb<Order>(root->left, callback, context)) return false;

        if (!callback(context, root->val))
          return false;

        if (root->right != nullptr)
          if (!inorder_traversal_cb<Order>(root->right, callback, context)) return false;
      } else {
        if (root->right != nullptr)
          if (!inorder_traversal_cb<Order>(root->right, callback, context)) return false;
        if (!callback(context, root->val))
          return false;
        if (root->left != nullptr)
          if (!inorder_traversal_cb<Order>(root->left, callback, context)) return false;
      }
      return true;
    }

    // Previous version of search() method passed all tests at
    // https://leetcode.com/problems/search-in-a-binary-search-tree/description/
    // need to test it again
    // TreeNode<T> need not to compare with T val, as long as T and T2 are comparable (i.e., T provides an override to compare with T2, e.g. Order vs int), it should work
    template<typename T2>
    static TreeNode<T> *search(TreeNode<T> *root, T2 val) {
      if (root == nullptr) {
        return nullptr;
      }
      if (root->val == val) {
        return root;
      }
      if (root->val > val) {
        return search(root->left, val);
      }
      return search(root->right, val);
    }

    // previous version of delete_node() method passed all tests at
    // https://leetcode.com/problems/delete-node-in-a-bst/description/
    // need to test it again
    template<typename T2>
    static bool delete_node(TreeNode<T> **root, T2 data) {
      if (*root == nullptr) {
        return false;
      }
      if (data < (*root)->val) {
        return delete_node(&((*root)->left), data);
      }
      if (data > (*root)->val) {
        return delete_node(&((*root)->right), data);
      }
      if ((*root)->val == data) {
        if ((*root)->left == nullptr && (*root)->right == nullptr) {
          delete *root;
          *root = nullptr;
          return true;
        }
        if ((*root)->left == nullptr || (*root)->right == nullptr) {
          // No need to look further, just promote the only child as
          // parent
          if ((*root)->left != nullptr) {
            TreeNode<T> *new_root = (*root)->left;
            delete *root;
            *root = new_root;
          } else {
            TreeNode<T> *new_root = (*root)->right;
            delete *root;
            *root = new_root;
          }
          return true;
        }
        TreeNode<T> **candidate_node = &((*root)->right);
        while ((*candidate_node)->left != nullptr) {
          candidate_node = &((*candidate_node)->left);
        }
        // We are not really deleting root, we are just
        // replacing its value and then deleting candidate_node
        (*root)->val = (*candidate_node)->val;
        return delete_node(candidate_node, (*candidate_node)->val);
      }
      return false;
    }

    // is_valid_bst() method passed all tests at
    // https://leetcode.com/problems/validate-binary-search-tree/description/
    static bool is_valid_bst(TreeNode<T> *root, int64_t min = INT64_MIN,
                             int64_t max = INT64_MAX) {
      if (root == nullptr)
        return true;
      if (root->val <= min || root->val >= max)
        return false;

      if (root->left != nullptr) {
        if (root->left->val < root->val) {
          if (!is_valid_bst(root->left, min, root->val))
            return false;
        } else {
          return false;
        }
      }
      if (root->right != nullptr) {
        if (root->val < root->right->val) {
          return is_valid_bst(root->right, root->val, max);
        }
        return false;
      }
      return true;
    }

    static void visualize_tree(TreeNode<T> *root, int depth = 0,
                               char branch = ' ') {
      if (root == nullptr) {
        return;
      }
      // Print right subtree
      visualize_tree(root->right, depth + 1, '/');

      // Print current node value with appropriate indentation
      std::print("{:>{}}{}{}\n", "", 6 * depth, branch, root->val);

      // Print left subtree
      visualize_tree(root->left, depth + 1, '\\');
    }
  };
} // namespace OrderBookProgrammingProblem

#endif // BST_LOCKED_IMPL_H
