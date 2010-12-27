#define PFUNC_PACK_CODE
#include <pfunc/pfunc.h>
#include <cstdarg>
#include <cstring>
#include <iostream>
#include <vector>

/**
 * \file pfunc_pack.cpp
 * \brief Implements pfunc_pack and pfunc_unpack functions
 *
 * While parallelizing a program, it is desirable to keep the original function 
 * as it is and write a wrapper around it that is the parallel call. As an example,
 * consider this:
 *
 * \code
 * void serial_foo (int a, int b, int c) {
 *   ...
 * }
 * 
 * void parallel_foo (void* buffer) {
 *   int a, int b, int c;
 *   pfunc_unpack (buffer, "int, int, int", &a, &b, &c);
 *   serial_foo (a, b, c);
 * }
 *
 * int main () {
 *   int a, b, c;
 *   void* buffer;
 *   ....
 *   pfunc_pack (&buffer, "int, int, int", a, b, c);
 *   pfunc_run (..,.., parallel_foo, buffer);
 *   ....
 * }
 * \endcode
 *
 * Notice that the big improvement here is that we have been able to keep serial_foo
 * as is. This is important since serial_foo might be part of a code that we cannot 
 * touch -- from another library. This is quintessential for non-intrusiveness.
 *
 */

enum {
  INVALID_TOKEN=-1, /**< The token is not one of the expected ones */
  INT, /**< int */
  UINT, /**< unsigned int */
  LINT, /**< long int */
  INTP, /**< int* */
  UINTP, /**< unsigned int* */
  LINTP, /**< long int* */
  INTPP, /**< int** */
  UINTPP, /**< unsigned int** */
  LINTPP, /**< long int** */
  CHRP, /**< char* */
  UCHRP, /**< unsigned char* */
  CHRPP, /**< char** */
  UCHRPP, /**< unsigned char** */
  FLTP, /**< float* */
  FLTPP, /**< float** */
  DBL, /**< double */
  DBLP, /**< double* */
  DBLPP, /**< double** */
  VOIDP /**< void* */
};

/**
 * static inline int get_star_count (const char*& format);
 *
 * \param[out] format The format string from which to extract the star count
 * \return The number of *'s found in this string till the ',' or End-Of-String
 */
static inline int get_star_count (const char*& format) {
  int star_count = 0;
  while (',' != *format && '\0' != *format) {
    switch (*format) {
      case ' ': /* harmless space */
                break;

      case '*': ++star_count;
                break;

      default: return INVALID_TOKEN;
               break;
    }
    ++format;
  }
  return star_count;
}

/**
 * \fn static inline int extract_tokens (const char* format, std::vector<int>& token_array);
 * \brief Extracts tokens from the format string and populates the given vector
 *
 * \param format A string that containts the desired format
 * \param token_array A std::vector of int's that is populated.
 *
 * \return The size of the buffer that needs to be allocated 
 * \return INVALID_TOKEN on error
 */
static inline int extract_tokens (const char* format, std::vector<int>& token_array) {
  int buffer_size = 0;

  while ('\0' != *format) {
    while (' ' == *format) ++format;
    switch (*format) {

      case 'i': /* has to lead to an 'int' 'int*' or 'int**' after spaces are removed */
                if ('n' != *++format || 't' != *++format) return INVALID_TOKEN;
                ++format;
                switch (get_star_count (format)) {
                  case 0: token_array.push_back (INT);
                          buffer_size += sizeof (int);
                          break;
                  case 1: token_array.push_back (INTP);
                          buffer_size += sizeof (int*);
                          break;
                  case 2: token_array.push_back (INTPP);
                          buffer_size += sizeof (int**);
                          break;
                  default: return INVALID_TOKEN;
                           break;
                }
                /* skip to the next token if there is one */
                if (',' == *format) ++format;
                break; /* finished processing */

      case 'c': /* has to lead to 'char*' or 'char**' after spaces */
                if ('h' != *++format || 'a' != *++format || 'r' != *++format) {
                  return INVALID_TOKEN;
                }
                ++format;
                switch (get_star_count (format)) {
                  case 1: token_array.push_back (UCHRP);
                          buffer_size += sizeof (unsigned char*);
                          break;
                  case 2: token_array.push_back (UCHRPP);
                          buffer_size += sizeof (unsigned char**);
                          break;
                  default: return INVALID_TOKEN;
                           break;
                }
                /* skip to the next token if there is one */
                if (',' == *format) ++format;
                break; /* finished processing */

      case 'd': /* has to lead to a 'double' or 'double*' or 'double**' after spaces */
                if ('o' != *++format || 'u' != *++format || 'b' != *++format ||
                    'l' != *++format || 'e' != *++format) return INVALID_TOKEN;
                ++format;
                switch (get_star_count (format)) {
                  case 0: token_array.push_back (DBL);
                          buffer_size += sizeof (double);
                          break;
                  case 1: token_array.push_back (DBLP);
                          buffer_size += sizeof (double*);
                          break;
                  case 2: token_array.push_back (DBLPP);
                          buffer_size += sizeof (double**);
                          break;
                  default: return INVALID_TOKEN;
                           break;
                }
                /* skip to the next token if there is one */
                if (',' == *format) ++format;
                break; /* finished processing */

      case 'v': /* has to lead to an 'void*' after spaces are removed */
                if ('o' != *++format || 'i' != *++format || 'd' != *++format)  {
                  return INVALID_TOKEN;
                }
                ++format;
                switch (get_star_count (format)) {
                  case 1: token_array.push_back (VOIDP);
                          buffer_size += sizeof (void*);
                          break;
                  default: return INVALID_TOKEN;
                           break;
                }
                /* skip to the next token if there is one */
                if (',' == *format) ++format;
                break; /* finished processing */

      case 'f': /* has to lead to an 'float*' or 'float**' after spaces are removed */
                if ('l' != *++format || 'o' != *++format || 
                    'a' != *++format || 't' != *++format)  {
                  return INVALID_TOKEN;
                }
                ++format;
                switch (get_star_count (format)) {
                  case 1: token_array.push_back (FLTP);
                          buffer_size += sizeof (float*);
                          break;
                  case 2: token_array.push_back (FLTPP);
                          buffer_size += sizeof (float**);
                          break;
                  default: return INVALID_TOKEN;
                           break;
                }
                /* skip to the next token if there is one */
                if (',' == *format) ++format;
                break; /* finished processing */

      case 'u': /* has to lead to the following after spaces 
                 * 1. unsigned int
                 * 2. unsigned int*
                 * 3. unsigned int**
                 * 4. unsigned char*
                 * 5. unsigned char**
                 */
                if ('n' != *++format || 's' != *++format || 'i' != *++format ||
                    'g' != *++format || 'n' != *++format || 'e' != *++format ||
                    'd' != *++format || ' ' != *++format) return INVALID_TOKEN;
                ++format;

                /* Now consume the extra white spaces */
                while (' ' == *format) ++format;

                /* Now, the current token has to be either an 'i' or a 'c' */
                switch (*format) {
                  case 'i':
                    if ('n' != *++format || 't' != *++format) return INVALID_TOKEN;
                    ++format;
                    switch (get_star_count (format)) {
                      case 0: token_array.push_back (UINT);
                              buffer_size += sizeof (unsigned int);
                              break;
                      case 1: token_array.push_back (UINTP);
                              buffer_size += sizeof (unsigned int*);
                              break;
                      case 2: token_array.push_back (UINTPP);
                              buffer_size += sizeof (unsigned int**);
                              break;
                      default: return INVALID_TOKEN;
                               break;
                    }
                    /* skip to the next token if there is one */
                    if (',' == *format) ++format;
                    break; /* finished processing */

                  case 'c':
                    if ('h' != *++format || 'a' != *++format || 'r' != *++format) {
                      return INVALID_TOKEN;
                    }
                    ++format;
                    switch (get_star_count (format)) {
                      case 1: token_array.push_back (UCHRP);
                              buffer_size += sizeof (unsigned char*);
                              break;
                      case 2: token_array.push_back (UCHRPP);
                              buffer_size += sizeof (unsigned char**);
                              break;
                      default: return INVALID_TOKEN;
                               break;
                    }
                    /* skip to the next token if there is one */
                    if (',' == *format) ++format;
                    break; /* finished processing */

                  default: return INVALID_TOKEN;
                           break;
                }

                break; /* finished processing */

      case 'l': /* has to lead to the following after spaces 
                 * 1. long int
                 * 2. long int*
                 * 3. long int**
                 */
                if ('o' != *++format || 'n' != *++format || 
                    'g' != *++format || ' ' != *++format) {
                  return INVALID_TOKEN;
                }
                ++format;

                /* Now consume the extra white spaces */
                while (' ' == *format) ++format;

                /* Now, the current token has to be either an 'i' or a 'c' */
                switch (*format) {
                  case 'i':
                    if ('n' != *++format || 't' != *++format) return INVALID_TOKEN;
                    ++format;
                    switch (get_star_count (format)) {
                      case 0: token_array.push_back (LINT);
                              buffer_size += sizeof (long int);
                              break;
                      case 1: token_array.push_back (LINTP);
                              buffer_size += sizeof (long int*);
                              break;
                      case 2: token_array.push_back (LINTPP);
                              buffer_size += sizeof (long int**);
                              break;
                      default: return INVALID_TOKEN;
                               break;
                    }
                    /* skip to the next token if there is one */
                    if (',' == *format) ++format;
                    break; /* finished processing */

                  default: return INVALID_TOKEN;
                           break;
                }

                break; /* finished processing */

      default: return INVALID_TOKEN;
    }
  }
  return buffer_size;
}

int pfunc_pack (char** buffer_ptr, const char* format, ...) {
  int buffer_size;
  std::vector<int> token_array;
  std::vector<int>::iterator iter;

  /* extract the tokens */
  if (INVALID_TOKEN == (buffer_size = extract_tokens (format, token_array))) {
    std::cout << "Error occured in extract_tokens" << std::endl;
    return PFUNC_ERROR;
  }

  /* Allocate the buffer */
  *buffer_ptr = new char[buffer_size];
  char* buffer = *buffer_ptr;

  if (NULL == buffer) {
    std::cout << "Could not allocate memory for the buffer" << std::endl;
    return PFUNC_NOMEM;
  }

  /* Now to interpret the buffer and copy the contents */
  iter = token_array.begin();
  va_list arg_ptr;
  va_start (arg_ptr, format);
  while (iter != token_array.end()) {
    switch (*iter) {
      case INT:
      {
        int param = va_arg (arg_ptr, int);
        memcpy (buffer, &param, sizeof(int));
        buffer += sizeof(int);
      }
      break;
      case INTP:
      {
        int* param = va_arg (arg_ptr, int*);
        memcpy (buffer, &param, sizeof(int*));
        buffer += sizeof (int*);
      }
      break;
      case INTPP:
      {
        int** param = va_arg (arg_ptr, int**);
        memcpy (buffer, &param, sizeof(int**));
        buffer += sizeof (int**);
      }
      break;
      case UINT:
      {
        unsigned int param = va_arg (arg_ptr, unsigned int);
        memcpy (buffer, &param, sizeof(unsigned int));
        buffer += sizeof (unsigned int);
      }
      break;
      case UINTP:
      {
        unsigned int* param = va_arg (arg_ptr, unsigned int*);
        memcpy (buffer, &param, sizeof(unsigned int*));
        buffer += sizeof (unsigned int*);
      }
      break;
      case UINTPP:
      {
        unsigned int** param = va_arg (arg_ptr, unsigned int**);
        memcpy (buffer, &param, sizeof(unsigned int**));
        buffer += sizeof (unsigned int**);
      }
      break;
      case LINT:
      {
        long int param = va_arg (arg_ptr, long int);
        memcpy (buffer, &param, sizeof(long int));
        buffer += sizeof (long int);
      }
      break;
      case LINTP:
      {
        long int* param = va_arg (arg_ptr, long int*);
        memcpy (buffer, &param, sizeof(long int*));
        buffer += sizeof (long int*);
      }
      break;
      case LINTPP:
      {
        long int** param = va_arg (arg_ptr, long int**);
        memcpy (buffer, &param, sizeof(long int**));
        buffer += sizeof (long int**);
      }
      break;
      case CHRP:
      {
        char* param = va_arg (arg_ptr, char*);
        memcpy (buffer, &param, sizeof(char*));
        buffer += sizeof (char*);
      }
      break;
      case CHRPP:
      {
        char** param = va_arg (arg_ptr, char**);
        memcpy (buffer, &param, sizeof(char**));
        buffer += sizeof (char**);
      }
      break;
      case UCHRP:
      {
        unsigned char* param = va_arg (arg_ptr, unsigned char*);
        memcpy (buffer, &param, sizeof(unsigned char*));
        buffer += sizeof (unsigned char*);
      }
      break;
      case UCHRPP:
      {
        unsigned char** param = va_arg (arg_ptr, unsigned char**);
        memcpy (buffer, &param, sizeof(unsigned char**));
        buffer += sizeof (unsigned char**);
      }
      break;
      case FLTP:
      {
        float* param = va_arg (arg_ptr, float*);
        memcpy (buffer, &param, sizeof(float*));
        buffer += sizeof (float*);
      }
      break;
      case FLTPP:
      {
        float** param = va_arg (arg_ptr, float**);
        memcpy (buffer, &param, sizeof(float**));
        buffer += sizeof (float**);
      }
      break;
      case DBL:
      {
        double param = va_arg (arg_ptr, double);
        memcpy (buffer, &param, sizeof(double));
        buffer += sizeof (double);
      }
      break;
      case DBLP:
      {
        double* param = va_arg (arg_ptr, double*);
        memcpy (buffer, &param, sizeof(double*));
        buffer += sizeof(double*);
      }
      break;
      case DBLPP:
      {
        double** param = va_arg (arg_ptr, double**);
        memcpy (buffer, &param, sizeof(double**));
        buffer += sizeof (double**);
      }
      break;
      case VOIDP:
      {
        void* param = va_arg (arg_ptr, void*);
        memcpy (buffer, &param, sizeof(void*));
        buffer += sizeof (void*);
      }
      break;
      default: 
        std::cout << "Error: format string not recognized" << std::endl;
        return PFUNC_ERROR;
    }
    ++iter;
  }
  va_end (arg_ptr);

  return PFUNC_SUCCESS;
}

int pfunc_unpack (char* buffer, const char* format, ...) {
  char* orig_buffer = buffer;
  std::vector<int> token_array;
  std::vector<int>::iterator iter;

  /* extract all the tokens */
  if (INVALID_TOKEN == extract_tokens (format, token_array)) {
    std::cout << "Error occured in extract_tokens" << std::endl;
    return PFUNC_ERROR;
  }

  /* Now to interpret the buffer and copy the contents */
  iter = token_array.begin();
  va_list arg_ptr;
  va_start (arg_ptr, format);
  while (iter != token_array.end()) {
    switch (*iter) {
      case INT:
      {
        int* param = va_arg (arg_ptr, int*);
        memcpy (param, buffer, sizeof(int));
        buffer += sizeof(int);
      } 
      break;
      case LINT:
      {
        long int* param = va_arg (arg_ptr, long int*);
        memcpy (param, buffer, sizeof(long int));
        buffer += sizeof (long int);
      } 
      break;
      case UINT:
      {
        unsigned int* param = va_arg (arg_ptr, unsigned int*);
        memcpy (param, buffer, sizeof(unsigned int));
        buffer += sizeof (unsigned int);
      } 
      break;
      case INTP:
      {
        int** param = va_arg (arg_ptr, int**);
        memcpy (param, buffer, sizeof(int*));
        buffer += sizeof (int*);
      } 
      break;
      case LINTP:
      {
        long int** param = va_arg (arg_ptr, long int**);
        memcpy (param, buffer, sizeof(long int*));
        buffer += sizeof (long int*);
      } 
      break;
      case UINTP:
      {
        unsigned int** param = va_arg (arg_ptr, unsigned int**);
        memcpy (param, buffer, sizeof(unsigned int*));
        buffer += sizeof (unsigned int*);
      } 
      break;
      case INTPP:
      {
        int*** param = va_arg (arg_ptr, int***);
        memcpy (param, buffer, sizeof(int**));
        buffer += sizeof (int**);
      } 
      break;
      case LINTPP:
      {
        long int*** param = va_arg (arg_ptr, long int***);
        memcpy (param, buffer, sizeof(long int**));
        buffer += sizeof (long int**);
      } 
      break;
      case UINTPP:
      {
        unsigned int*** param = va_arg (arg_ptr, unsigned int***);
        memcpy (param, buffer, sizeof(unsigned int**));
        buffer += sizeof (unsigned int**);
      } 
      break;
      case CHRP:
      {
        char** param = va_arg (arg_ptr, char**);
        memcpy (param, buffer, sizeof(char*));
        buffer += sizeof (char*);
      } 
      break;
      case UCHRP:
      {
        unsigned char** param = va_arg (arg_ptr, unsigned char**);
        memcpy (param, buffer, sizeof(unsigned char*));
        buffer += sizeof (unsigned char*);
      } 
      break;
      case CHRPP:
      {
        char*** param = va_arg (arg_ptr, char***);
        memcpy (param, buffer, sizeof(char**));
        buffer += sizeof (char**);
      } 
      break;
      case UCHRPP:
      {
        unsigned char*** param = va_arg (arg_ptr, unsigned char***);
        memcpy (param, buffer, sizeof(unsigned char**));
        buffer += sizeof (unsigned char**);
      }  
      break;
      case FLTP:
      {
        float** param = va_arg (arg_ptr, float**);
        memcpy (param, buffer, sizeof(float*));
        buffer += sizeof (float*);
      } 
      break;
      case FLTPP:
      {
        float*** param = va_arg (arg_ptr, float***);
        memcpy (param, buffer, sizeof(float**));
        buffer += sizeof (float**);
      } 
      break;
      case DBL:
      {
        double* param = va_arg (arg_ptr, double*);
        memcpy (param, buffer, sizeof(double));
        buffer += sizeof (double);
      } 
      break;
      case DBLP:
      {
        double** param = va_arg (arg_ptr, double**);
        memcpy (param, buffer, sizeof(double*));
        buffer += sizeof(double*);
      } 
      break;
      case DBLPP:
      {
        double*** param = va_arg (arg_ptr, double***);
        memcpy (param, buffer, sizeof(double**));
        buffer += sizeof (double**);
      } 
      break;
      case VOIDP:
      {
        void** param = va_arg (arg_ptr, void**);
        memcpy (param, buffer, sizeof(void*));
        buffer += sizeof (void*);
      } 
      break;
      default:
        std::cout << "Error: format string not recognized" << std::endl;
        return PFUNC_ERROR;
    }
    ++iter;
  }
  va_end (arg_ptr);

  delete [] orig_buffer;

  return PFUNC_SUCCESS;
}
