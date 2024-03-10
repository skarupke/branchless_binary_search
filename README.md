# Branchless Binary Search

This is a fast binary search implementation. See the blog post here: https://probablydance.com/2023/04/27/beautiful-branchless-binary-search/

If your compiler does not compile the branch in the loop to a CMOV instruction, complain to your compiler or try to get CMOV into the C++ standard.

![alt text](benchmark.png)