#pragma once

#include <ir/Instruction.hh>
#include <ir/Value.hh>

#include <concepts>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace ir {

class BasicBlockIterator;

class BasicBlock final : public Value {
    std::vector<std::unique_ptr<Instruction>> m_instructions;

public:
    BasicBlock() : Value(ValueKind::BasicBlock) {}
    BasicBlock(const BasicBlock &) = delete;
    BasicBlock(BasicBlock &&) = delete;
    ~BasicBlock() override;

    BasicBlock &operator=(const BasicBlock &) = delete;
    BasicBlock &operator=(BasicBlock &&) = delete;

    BasicBlockIterator begin() const;
    BasicBlockIterator end() const;
    BasicBlockIterator iterator(Instruction *position) const;

    template <std::derived_from<Instruction> Inst, typename... Args>
    Inst *insert(BasicBlockIterator position, Args &&...args);

    template <std::derived_from<Instruction> Inst, typename... Args>
    Inst *insert(Instruction *position, Args &&...args);

    template <std::derived_from<Instruction> Inst, typename... Args>
    Inst *prepend(Args &&...args);

    template <std::derived_from<Instruction> Inst, typename... Args>
    Inst *append(Args &&...args);
};

class BasicBlockIterator {
    const std::vector<std::unique_ptr<Instruction>> *m_instructions;
    std::size_t m_index;

public:
    BasicBlockIterator(const std::vector<std::unique_ptr<Instruction>> &instructions, std::size_t index)
        : m_instructions(&instructions), m_index(index) {}

    BasicBlockIterator &operator++() {
        m_index++;
        return *this;
    }
    BasicBlockIterator &operator--() {
        m_index--;
        return *this;
    }

    bool operator<=>(const BasicBlockIterator &) const = default;
    Instruction *operator*() const { return (*m_instructions)[m_index].get(); }
    std::size_t index() const { return m_index; }
};

inline BasicBlockIterator BasicBlock::begin() const {
    return {m_instructions, 0};
}

inline BasicBlockIterator BasicBlock::end() const {
    return {m_instructions, m_instructions.size()};
}

inline BasicBlockIterator BasicBlock::iterator(Instruction *position) const {
    std::size_t index = 0;
    for (auto *inst : *this) {
        if (inst == position) {
            break;
        }
        index++;
    }
    return {m_instructions, index};
}

template <std::derived_from<Instruction> Inst, typename... Args>
Inst *BasicBlock::insert(BasicBlockIterator position, Args &&...args) {
    return static_cast<Inst *>(
        m_instructions.emplace(m_instructions.begin() + position.index(), new Inst(std::forward<Args>(args)...))
            ->get());
}

template <std::derived_from<Instruction> Inst, typename... Args>
Inst *BasicBlock::insert(Instruction *position, Args &&...args) {
    return insert<Inst>(iterator(position), std::forward<Args>(args)...);
}

template <std::derived_from<Instruction> Inst, typename... Args>
Inst *BasicBlock::prepend(Args &&...args) {
    return insert<Inst>(begin(), std::forward<Args>(args)...);
}

template <std::derived_from<Instruction> Inst, typename... Args>
Inst *BasicBlock::append(Args &&...args) {
    return insert<Inst>(end(), std::forward<Args>(args)...);
}

} // namespace ir
