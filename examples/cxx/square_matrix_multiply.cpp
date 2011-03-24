/**
 * @author Prabhanjan Kambadur
 * 
 * A program that computes the product of two matrices using divide and
 * conquer. Here is the brief sketch of how we go about computing A*B.
 * 
 * Let A = A11 | A12    and  B = B11 | B12
 *         _________             _________
 *         A21 | A22             B21 | B22
 *                      
 * Then, C = A*B = C11 | C12
 *                 ---------
 *                 C21 | C22
 * 
 * Where, C11 = A11*B11 + A12*B21
 *        C12 = A11*B12 + A12*B22
 *        C21 = A21*B21 + A22*B21
 *        C22 = A21*B22 + A22*B22
 *
