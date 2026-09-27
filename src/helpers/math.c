#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include "math.h"


int next_prime(int num) {
    if (num == 0 || num == 1)
        return 2;

    int SEARCH_LIMIT = 2*num;

    for (int p_candidate = num+1; p_candidate < SEARCH_LIMIT; ++ p_candidate) {
        bool can_be_prime = true;
        for (int div_candidate = 2; div_candidate*div_candidate <= p_candidate; ++ div_candidate) {
            if (p_candidate % div_candidate == 0) {
                can_be_prime = false;
                break;
            }
        }

        if (can_be_prime)
            return p_candidate;
    }

    assert(false && "No prime found within the guaranteed range; this should never happen");
}