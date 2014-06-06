#ifndef _CIRCLE_QUEUE_H_
#define _CIRCLE_QUEUE_H_

#include <stdint.h>
#include <stdio.h>

/**
 * @brief һ��������ѭ������
 */
template<typename DataType>
class CCircleQueue {
public:
    /**
     * @brief ���ʼ������
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
     * @brief �����������
     */
    ~CCircleQueue() {destroy();}

    /**
     * @brief ��ʼ�����У�������������Ϊsize��
     *        ���size����2���ݵģ���ʵ���趨�Ķ�������
     *        Ϊ�����ڵ���size����С��2����
     *
     * @param size һ���޷��ŵ����� > 0
     *
     * @return 0:�ɹ�  ����:ʧ��
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
        destroy();              // ���֮ǰ�Ķ���
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
        // ��ձ�־λ
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
     * @brief ������item���뵽������
     *
     * @param item �����������
     *
     * @return true:�ɹ�  false:ʧ��
     */
    bool push(const DataType& item) {
        if(__sync_fetch_and_add(&m_size2, 1) >= m_capacity) {
            __sync_sub_and_fetch(&m_size2, 1);
            return false;       // ��������
         }
        int  tail_idx = __sync_fetch_and_add(&m_tail_idx, 1);
        tail_idx &= m_mask;
        volatile uint32_t& bit_tag = m_bit_tag[ tail_idx >> 5 ];
        uint32_t  tmp_bit = 1U<<(tail_idx & 31);
        while( bit_tag & tmp_bit ); // �ȴ� pop ��� 
        m_data[tail_idx] = item;
        __sync_or_and_fetch(&bit_tag, tmp_bit); // ���ñ�־λ
        __sync_add_and_fetch(&m_size, 1);
        return true;
    }

    /**
     * @brief �Ӷ�����pop��һ�����ݣ�����ֵ��item
     *
     * @param item ���ڴ洢pop������
     *
     * @return true:�ɹ�  false:ʧ��
     */
    bool pop(DataType& item) {
        if(__sync_fetch_and_sub(&m_size, 1) <= 0) {         // ����Ϊ��
            __sync_add_and_fetch(&m_size, 1);
            return false;
        }
        int head_idx = __sync_fetch_and_add(&m_head_idx, 1);
        head_idx &= m_mask;
        volatile uint32_t& bit_tag = m_bit_tag[ head_idx >> 5 ];
        uint32_t  tmp_bit = 1U << (head_idx & 31);
        while( tmp_bit & ~bit_tag ); // �ȴ� push ���
        item = m_data[head_idx];
        __sync_and_and_fetch(&bit_tag, ~(1U<<(head_idx & 31))); // �����־λ
        __sync_sub_and_fetch(&m_size2, 1);
        return true;
    }

    /**
     * @brief �ж϶����Ƿ�Ϊ��
     *
     * @return true:����Ϊ��  false:���в�Ϊ��
     */
    bool  empty() {return  m_size <= 0;}
protected:
    /**
     * @brief ��ն���
     *
     * @return 0:�ɹ� ����:ʧ��
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
    DataType* volatile   m_data;         		  // ���ݴ洢��
    volatile uint32_t* volatile   m_bit_tag;      // �����Ƿ�Ϊ�յı�ʶ 0:Ϊ�� 1:��Ϊ��
    volatile unsigned int         m_capacity;     // ���е�����
    volatile int                  m_size;
    volatile int                  m_size2;
    volatile unsigned int         m_mask;
    volatile unsigned int         m_head_idx;     // ����ͷ���±�
    volatile unsigned int         m_tail_idx;     // ����β���±�
};

#endif
