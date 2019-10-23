#ifndef _CIRCLE_QUEUE_H_
#define _CIRCLE_QUEUE_H_

#include <stdint.h>
#include <stdio.h>

/**
 * @brief 一个无锁的循环队列
 */
template<typename DataType>
class CCircleQueue {
public:
    /**
     * @brief 类初始化函数
     */
    CCircleQueue() {
        m_data = NULL;
        m_bit_tag = NULL;
        m_capacity = 0;
        m_size = 0;
        m_size2 = 0;
        m_head_idx = 0;
        m_tail_idx = 0;
    }

    /**
     * @brief 类的析构函数
     */
    ~CCircleQueue() {destroy();}

    /**
     * @brief 初始化队列，将队列容量设为size。
     *        如果size不是2的幂的，则实际设定的队列容量
     *        为：大于等于size的最小的2的幂
     *
     * @param size 一个无符号的整数 > 0
     *
     * @return 0:成功  其它:失败
     */
    int  init(unsigned size) {
        if(!size) return -1;
        if(size & (size-1)) {
            // Round up to the next highest power of 2
            for(unsigned i=1, n=(sizeof size)<<3;
                i<n; i<<=1) {
                size |= size>>i;
            }
            ++size;
        }
        destroy();              // 清空之前的队列
        m_data = new DataType[size];
        if(!m_data) {
            // out of memroy!
            return -2;
        }
        m_bit_tag = new volatile uint32_t[(size>>5) + 1];
        if(!m_bit_tag) {
            // out of memory!
            delete [] m_data;
            m_data = NULL;
            return -2;
        }
        // 清空标志位
        for(unsigned i=0,n=(size>>5)+1;
            i < n; ++i) {
            m_bit_tag[i] = 0;
        }
        m_capacity = size;
        m_size = 0;
        m_size2 = 0;
        m_head_idx = 0;
        m_tail_idx = 0;
        m_mask = size-1;
        return 0;
    }

    /**
     * @brief 将数据item加入到队列中
     *
     * @param item 待加入的数据
     *
     * @return true:成功  false:失败
     */
    bool push(const DataType& item) {
        if(__sync_fetch_and_add(&m_size2, 1) >= m_capacity) {
            __sync_sub_and_fetch(&m_size2, 1);
            return false;       // 队列满了
         }
        int  tail_idx = __sync_fetch_and_add(&m_tail_idx, 1);
        tail_idx &= m_mask;
        volatile uint32_t& bit_tag = m_bit_tag[ tail_idx >> 5 ];
        uint32_t  tmp_bit = 1U<<(tail_idx & 31);
        while( bit_tag & tmp_bit ); // 等待 pop 完成 
        m_data[tail_idx] = item;
        __sync_or_and_fetch(&bit_tag, tmp_bit); // 设置标志位
        __sync_add_and_fetch(&m_size, 1);
        return true;
    }

    /**
     * @brief 从队列中pop出一个数据，并赋值给item
     *
     * @param item 用于存储pop的数据
     *
     * @return true:成功  false:失败
     */
    bool pop(DataType& item) {
        if(__sync_fetch_and_sub(&m_size, 1) <= 0) {         // 队列为空
            __sync_add_and_fetch(&m_size, 1);
            return false;
        }
        int head_idx = __sync_fetch_and_add(&m_head_idx, 1);
        head_idx &= m_mask;
        volatile uint32_t& bit_tag = m_bit_tag[ head_idx >> 5 ];
        uint32_t  tmp_bit = 1U << (head_idx & 31);
        while( tmp_bit & ~bit_tag ); // 等待 push 完成
        item = m_data[head_idx];
        __sync_and_and_fetch(&bit_tag, ~(1U<<(head_idx & 31))); // 清除标志位
        __sync_sub_and_fetch(&m_size2, 1);
        return true;
    }

    /**
     * @brief 判断队列是否为空
     *
     * @return true:队列为空  false:队列不为空
     */
    bool  empty() {return  m_size <= 0;}
protected:
    /**
     * @brief 清空队列
     *
     * @return 0:成功 其它:失败
     */
    int  destroy() {
        if(m_data) {
            delete [] m_data;
            m_data = NULL;
        }
        if(m_bit_tag) {
            delete [] m_bit_tag;
            m_bit_tag = NULL;
        }
        m_capacity = 0;
        m_size = 0;
        m_size2 = 0;
        m_mask = 0;
        m_head_idx = 0;
        m_tail_idx = 0;
        return 0;
    }
private:
    DataType* volatile   m_data;         		  // 数据存储区
    volatile uint32_t* volatile   m_bit_tag;      // 数据是否为空的标识 0:为空 1:不为空
    volatile unsigned int         m_capacity;     // 队列的容量
    volatile int                  m_size;
    volatile int                  m_size2;
    volatile unsigned int         m_mask;
    volatile unsigned int         m_head_idx;     // 队列头的下标
    volatile unsigned int         m_tail_idx;     // 队列尾的下标
};

#endif
