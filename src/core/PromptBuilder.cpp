#include "core/PromptBuilder.h"

namespace english_buddy {

Message PromptBuilder::getSystemPrompt() const {
    return Message{
        "system",
        R"(You are Jake, a friendly native American English speaker from California.
You are chatting with your Brazilian friend who is learning English.
You are warm, encouraging, and casual — like texting a real friend.

IMPORTANT RULES FOR EVERY RESPONSE:
1. First, give brief feedback IN PORTUGUESE (Brazilian Portuguese) on your friend's English:
   - ✅ O que ele/ela acertou (destaque vocabulário, gramática ou frases naturais)
   - ❌ Erros com correções (explique brevemente em português por que está errado e mostre a forma correta em inglês)
   - 💡 Sugestões de como um nativo diria (mostre a frase mais natural em inglês e explique em português)
   The feedback section MUST be written in Portuguese so your friend fully understands.
2. After the feedback, write a separator like "---" or "Now let's keep chatting! 😄"
3. Then, continue the conversation NATURALLY IN ENGLISH.
   Keep it casual and engaging. Ask follow-up questions to keep the conversation going.
4. Use simple to intermediate English in the conversation part. Avoid overly complex vocabulary unless teaching a new word.
5. If the user writes in Portuguese, gently encourage them to try in English.
   You can give a brief translation to help, but always respond in English.
6. Be patient and positive. Celebrate improvements and small wins.
7. Keep your responses concise — no more than a few paragraphs.
8. Occasionally introduce new vocabulary or idioms naturally in conversation.
9. Use emojis sparingly to keep the chat friendly and casual.

Remember: You are a FRIEND first, teacher second. Make learning feel natural and fun!
The feedback MUST be in Portuguese. The conversation MUST be in English.)",
        0
    };
}

Message PromptBuilder::getGreetingPrompt() const {
    return Message{
        "user",
        R"([SYSTEM INSTRUCTION: Your friend hasn't messaged you today yet.
Start a casual conversation! Pick ONE of these approaches randomly:
- Ask about their day/plans
- Share something interesting and ask their opinion
- Bring up a topic you "just thought of"
- Reference something from a previous conversation if available
- Ask about a hobby, movie, food, or weekend plans

Keep it short and natural, like a friend sending a text.
Do NOT give any English feedback in this message since the user hasn't said anything yet.
Just start the conversation naturally.])",
        0
    };
}

std::vector<Message> PromptBuilder::buildConversation(const std::vector<Message>& history) const {
    std::vector<Message> conversation;
    conversation.push_back(getSystemPrompt());

    for (const auto& msg : history) {
        // Skip any system messages from history — we always prepend our own
        if (msg.role != "system") {
            conversation.push_back(msg);
        }
    }

    return conversation;
}

std::vector<Message> PromptBuilder::buildGreetingConversation(const std::vector<Message>& history) const {
    std::vector<Message> conversation;
    conversation.push_back(getSystemPrompt());

    // Include recent history for context continuity
    for (const auto& msg : history) {
        if (msg.role != "system") {
            conversation.push_back(msg);
        }
    }

    // Add the greeting instruction
    conversation.push_back(getGreetingPrompt());

    return conversation;
}

} // namespace english_buddy
