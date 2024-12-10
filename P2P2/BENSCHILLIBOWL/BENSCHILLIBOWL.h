#ifndef LAB3_BENSCHILLIBOWL_H_
#define LAB3_BENSCHILLIBOWL_H_

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


// Let a menu item

typedef char* MenuItem;

typedef struct OrderStruct {
    MenuItem menu_item;
    int customer_id;
    int order_number;
    struct OrderStruct *next;
} Order;

typedef struct Restaurant {
    Order* orders;
    int current_size;
    int max_size;
    int next_order_number;
    int orders_handled;
	int expected_num_orders;
    pthread_mutex_t mutex;
    pthread_cond_t can_add_orders, can_get_orders;
} BENSCHILLIBOWL;

/**
 * Picks a random menu item and returns it.
 */
MenuItem PickRandomMenuItem();

/**
 * Creates a restaurant with a maximum size and the expected number of orders.
 * Returns the restaurant.
 * 
 * This function should:
 *  - allocate space for the restaurant
 *  - initialize all its variables
 *  - initialize its synchronization objects
 */
BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders);

/**
 * Closes the restaurant. This function should:
 *  - ensure all orders have been fulfilled
 *  - ensure the number of orders fulfilled matches the expected number of orders
 *  - destroy all the synchronization objects
 *  - free the space of the restaurant
 */
void CloseRestaurant(BENSCHILLIBOWL* mcg);
  
/**
 * Add an order to the restaurant. This function should:
 *  - Wait until the restaurant is not full
 *  - Add an order to the back of the orders queue
 *  - populate the order number of the order
 *  - return the order number
 */
int AddOrder(BENSCHILLIBOWL* mcg, Order* order);

/**
 * Gets an order from the restaurant. This funtion should:
 *  - Wait until the restaurant is not empty
 *  - get an order from the front of the orders queue
 *  - return the order
 * 
 * If there are no orders left, this function should notify the other cooks
 * that there are no orders left.
 */
Order *GetOrder(BENSCHILLIBOWL* mcg);

#endif 