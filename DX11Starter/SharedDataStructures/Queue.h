#pragma once

template<typename T, int max>
class Queue {
	T m_data[max];
	int m_head;
	int m_tail;
	int m_count;

public:
	Queue() {
		m_head = 0;
		m_tail = 0;
		m_count = 0;
	}

	// Pushes a va
	bool Push(const T& dataIn) {
		if (m_count < max) {
			m_data[m_tail] = data;
			m_tail = (m_tail + 1) % max;
			m_count++;
			return true;
		}
		else {
			return false;
		}
	}

	bool Pop(T& dataOut) {
		if (m_count > 0) {
			dataOut = m_data[m_head];
			m_head = (m_head + 1) % max;
			m_count--;
			return true;
		}
		else {
			return false;
		}
	}

	int GetCount() {
		return m_count;
	}
};