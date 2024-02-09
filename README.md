# Process And Thread Management

Matrix multiplication is a fundamental operation in linear algebra and has applications in various domains, such as computer graphics, scientific computing, and machine learning. This Task explores different approaches to optimize matrix multiplication in C, ranging from a naive serial implementation to parallel implementations using processes and threads.

The primary objective is to compare the performance of different approaches to matrix multiplication, including a naive serial solution, a process-based solution, and various thread-based solutions. The focus is on achieving optimal execution time while maintaining correct results.

## Implemented Approaches:

1- **Naive Approach**  
    
  The naive approach represents a conventional method of matrix multiplication,using nested loops. It serves as a baseline for performance comparison.
    
2- **Process-Based Approach**
    
    
  The process-based approach involves dividing the matrix multiplication task among multiple processes. Each process is responsible for a subset of the rows, and communication between processes is achieved using inter-process communication (IPC) via pipes.
    
3- **Thread-Based Approaches**  
  - 3.1 Join Threads
    This approach uses multiple joinable threads to parallelize the matrix multiplication task. Each thread is assigned a subset of rows to compute, and results are combined upon thread completion.
  - 3.2 Mix of Join and Detached Threads
    This approach combines joinable and detached threads to exploit the benefits of both. The detached threads focus on completing their tasks independently, while the joinable threads ensure synchronization before completing the entire computation.
  - 3.3 Detached Threads
    This approach employs detached threads, allowing them to run independently without the need for explicit synchronization. The results are checked for correctness after completion.


## Sample run
<p align="center">
  <img width="720" height="405" src="">
</p>

## 🔗 Links

[![facebook](https://img.shields.io/badge/facebook-0077B5?style=for-the-badge&logo=facebook&logoColor=white)](https://www.facebook.com/qossay.rida?mibextid=2JQ9oc)

[![Whatsapp](https://img.shields.io/badge/Whatsapp-25D366?style=for-the-badge&logo=Whatsapp&logoColor=white)](https://wa.me/+972598592423)

[![linkedin](https://img.shields.io/badge/linkedin-0077B5?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/qossay-rida-3aa3b81a1?utm_source=share&utm_campaign=share_via&utm_content=profile&utm_medium=android_app )

[![twitter](https://img.shields.io/badge/twitter-1DA1F2?style=for-the-badge&logo=twitter&logoColor=white)](https://twitter.com/qossayrida)

