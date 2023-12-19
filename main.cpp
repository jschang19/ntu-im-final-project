#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h> 
#include <thread>
#include <chrono>
#include <cstdlib>  // For getenv
#include "ChatGPT/include/Error.h"
#include "ChatGPT/include/Game.h"
// this is the main function

const int STORY_NUM=5;

int main(){
    std::cout << "\033[2J\033[H";
    System::Game game;
    const char* openaiKey = std::getenv("OPENAI_API_KEY");
    if(openaiKey == nullptr){
        game.print("沒有 OpenAI API Key，請在參數後方輸入你的 API key","r", true);
        game.print("範例：./ChatGPT <YOUR_API_KEY>");
        game.print("請至 https://platform.openai.com/api-keys 取得 API key");
        return 0;
    }

    std::string key = openaiKey;
    game.print("\U0001F389 歡迎來到 NTU 模擬器！","w", true);
    game.checkStatus(key);
    std::cout<<std::endl;

    game.count=STORY_NUM;
    OpenAI::ChatGPT chatGpt{key};
    game.printWelcome();
    // get random story ids
    std::vector<int> story_ids = game.getRandStoryIds(STORY_NUM);
    game.story_ids = story_ids;

    try {
        for (int i=0; i<STORY_NUM; i++){
            std::cout << "\033[2J\033[H";
            int story_id = story_ids[i];
            OpenAI::Message prompt = game.generateStoryPrompt(i);
            game.addPrompt(chatGpt, prompt, false);
            auto chatCompletion = game.sendToChatGPT(chatGpt, false);
            game.parseGPTResponse(chatCompletion, story_id);
            std::cout << "\033[2J\033[H";
    
            game.printOptions(story_id);
            std::cout<<std::endl;

            std::string userInput;
            std::cout<<"你的選擇是（ 輸入英文小寫代號 ）:";
            std::getline(std::cin,userInput);
            std::cout<<std::endl;
            game.print("你選擇了 "+userInput, "w");
            game.setUserChoice(story_id, userInput);
            game.current_count += 1;
            std::cout<<std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }catch(OpenAI::Error& e){
        //JSON error returned by the server
        std::cout<<e.what();
    }
    OpenAI::Message final_prompt = game.generateEndingPrompt();
    game.addPrompt(chatGpt, final_prompt, true);
    auto chatCompletion = game.sendToChatGPT(chatGpt, true);
    game.parseEndingResponse(chatCompletion);

    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "\033[2J\033[H";
    game.print("遊戲結束！感謝你的遊玩","w", true);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    game.print("本遊戲由第 48 小組製作，以及 OpenAI 提供技術支援","l");
    game.print("本專案架構參考 deni2312/ChatGPT-Cpp 擴充開發","l");
    return 0;
}