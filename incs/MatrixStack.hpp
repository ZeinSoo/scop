#pragma once

#include <vector>

#include "maths.hpp"

class MatrixStack {
    private:
        std::vector<Mat4> stack;

    public:
        MatrixStack() {
            stack.emplace_back(1.f);
        }

        void loadIdentity() {
            stack.clear();
            stack.emplace_back(1.f);
        }

        void push() {
            stack.push_back(stack.back());
        }

        void pop() {
            if (stack.size() > 1)
                stack.pop_back();
        }

        Mat4 &top() {
            return stack.back();
        }

        const Mat4 &top() const {
            return stack.back();
        }
};
