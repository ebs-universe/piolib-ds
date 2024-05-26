/* 
   Copyright (c)
     (c) 2015-2016 Chintalagiri Shashank, Quazar Technologies Pvt. Ltd.
   
   This file is part of
   Embedded bootstraps : data structures library
   
   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. 
*/

/**
 * @file avltree.c
 * @brief Implementation of the AVL binary search tree.
 * 
 * Based on the implemenation at https://gist.github.com/tonious/1377768
 *
 * @see avltree.h
 */

#include "ds/avltree.h"


static AVLT_HEIGHT_t _node_height(avlt_node_t * node);
static AVLT_HEIGHT_t _node_balance_factor(avlt_node_t *node);

static avlt_node_t * _node_rotate_leftleft(avlt_node_t *node);
static avlt_node_t * _node_rotate_leftright(avlt_node_t *node);
static avlt_node_t * _node_rotate_rightleft(avlt_node_t *node);
static avlt_node_t * _node_rotate_rightright(avlt_node_t *node);

static avlt_node_t * _node_balance(avlt_node_t * node);
static void _tree_balance(avlt_t *tree);


void avlt_init_node(avlt_node_t * node, uint16_t key, void * content){
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    node->content = content;
}


static AVLT_HEIGHT_t _node_height(avlt_node_t * node){
    AVLT_HEIGHT_t height_left = 0;
    AVLT_HEIGHT_t height_right = 0;
    if( node->left ) {
        height_left = _node_height( node->left );
    }
    if( node->right ) {
        height_right = _node_height( node->right );
    }
    return height_right > height_left ? ++height_right : ++height_left;
}

static AVLT_HEIGHT_t _node_balance_factor(avlt_node_t * node){
    AVLT_HEIGHT_t bf = 0;
    if(node->left ){
        bf += _node_height( node->left );
    }
    if(node->right){
        bf -= _node_height( node->right );
    }
    return bf ;
}

static avlt_node_t * _node_rotate_leftleft(avlt_node_t * node){
    avlt_node_t * a = node;
    avlt_node_t * b = a->left;
    
    a->left = b->right;
    b->right = a;
    
    return(b);
}

static avlt_node_t * _node_rotate_leftright(avlt_node_t * node){
    avlt_node_t * a = node;
    avlt_node_t * b = a->left;
    avlt_node_t * c = b->right;
    
    a->left = c->right;
    b->right = c->left; 
    c->left = b;
    c->right = a;
    
    return(c);
}

static avlt_node_t * _node_rotate_rightleft(avlt_node_t * node){
    avlt_node_t * a = node;
    avlt_node_t * b = a->right;
    avlt_node_t * c = b->left;
    
    a->right = c->left;
    b->left = c->right; 
    c->right = b;
    c->left = a;
    
    return(c);
}

static avlt_node_t * _node_rotate_rightright(avlt_node_t * node){
    avlt_node_t * a = node;
    avlt_node_t * b = a->right;
    
    a->right = b->left;
    b->left = a; 
    
    return(b);
}

static avlt_node_t * _node_balance(avlt_node_t * node){
    avlt_node_t * newroot = NULL;
    
    /* Balance node's children, if they exist. */
    if(node->left){
        node->left = _node_balance(node->left);
    }   
    if(node->right){
        node->right = _node_balance(node->right);
    }
        
    AVLT_HEIGHT_t bf = _node_balance_factor(node);
    
    if(bf >= 2){
        /* Left Heavy */    
        if(_node_balance_factor(node->left) <= -1) 
            newroot = _node_rotate_leftright(node);
        else 
            newroot = _node_rotate_leftleft(node);
    } 
    else if(bf <= -2){
        /* Right Heavy */
        if(_node_balance_factor(node->right) >= 1)
            newroot = _node_rotate_rightleft(node);
        else 
            newroot = _node_rotate_rightright(node);
    } 
    else {
        /* This node is balanced -- no change. */
        newroot = node;
    }
    
    return(newroot);  
}

static void _tree_balance(avlt_t *tree){
    avlt_node_t *newroot = NULL;
    newroot = _node_balance(tree->root);
    if(newroot != tree->root){
        tree->root = newroot;
    }
}

void avlt_insert_node(avlt_t * tree, avlt_node_t * node){
    avlt_node_t *next = NULL;
    avlt_node_t *last = NULL;
    if(tree->root == NULL){
        /* Well, there must be a first node */  
        tree->root = node;
    } 
    else{
        /* Okay.  We have a root already.  Where do we put this? */
        next = tree->root;
        while(next != NULL){
            last = next;
            if(node->key < next->key){
                next = next->left;
            }
            else if(node->key > next->key){
                next = next->right;
            } 
            else if(node->key == next->key) {
                /* Have we already inserted this node? */
                /* This shouldn't happen. */    
            }
        }
        if(node->key < last->key){
            last->left = node;
        }
        if(node->key > last->key){
            last->right = node;
        }
    }
    _tree_balance(tree);
}

avlt_node_t * avlt_find_node(avlt_t * tree, uint16_t key){
    avlt_node_t * current = tree->root;
    while(current && current->key!= key){
        if(key > current->key)
            current = current->right;
        else
            current = current->left;
    }
    return current;
}
