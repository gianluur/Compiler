/*
MIT License

Copyright (c) 2024 Gianluca Russo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <iostream>
#include <string>
#include <unordered_map> 

#include <cstdint> //for the uint8_t type
#include <cstdlib> //for the exit function

#ifdef _WIN32
#include <windows.h>
#else
#endif

using std::cout, std::endl;
using std::string, std::to_string, std::unordered_map;
using std::ostream;

//Check if the user is on Windows, if true includes the library for the Windows API otherwise it doesn't

namespace CLIStyle {
  
  //This namespace contains all the functions that the user shouldn't access

  namespace _private {

    //Maps style names to their corresponding ANSI escape codes.
    unordered_map<string, string> styles = 
    { 
      {"bold", "\033[1m"},
      {"italic", "\033[3m"}, 
      {"underline", "\033[4m"}, 
      {"reverse", "\033[7m"} 
    };

    //Maps color names to their corresponding ANSI escape codes for text.
    unordered_map<string, string> color_text = {
      {"grey", "\033[30m"},
      {"red", "\033[31m"},
      {"green", "\033[32m"},
      {"yellow", "\033[33m"},
      {"blue", "\033[34m"},
      {"magenta", "\033[35m"},
      {"cyan", "\033[36m"},
      {"white", "\033[37m"},
      {"bright grey", "\033[1;30m"},
      {"bright red", "\033[1;31m"},
      {"bright green", "\033[1;32m"},
      {"bright yellow", "\033[1;33m"},
      {"bright blue", "\033[1;34m"},
      {"bright magenta", "\033[1;35m"},
      {"bright cyan", "\033[1;36m"},
      {"bright white", "\033[1;37m"}
    };

    //Maps color names to their corresponding ANSI escape codes for background.
    unordered_map<string, string> color_background = {
      {"grey", "\033[40m"},
      {"red", "\033[41m"},
      {"green", "\033[42m"},
      {"yellow", "\033[43m"},
      {"blue", "\033[44m"},
      {"magenta", "\033[45m"},
      {"cyan", "\033[46m"},
      {"white", "\033[47m"},
      {"bright grey", "\033[1;40m"},
      {"bright red", "\033[1;41m"},
      {"bright green", "\033[1;42m"},
      {"bright yellow", "\033[1;43m"},
      {"bright blue", "\033[1;44m"},
      {"bright magenta", "\033[1;45m"},
      {"bright cyan", "\033[1;46m"},
      {"bright white", "\033[1;47m"}
    };

    constexpr auto RESET_STYLE = "\033[0m";

    bool handleVTSequences = false; //checks if the user terminal can access the ANSI code

    #ifdef _WIN32 // check if the user is on Windows
    /**
     * @brief Enables virtual terminal sequence processing for the console output.
     * 
     * @param handleVTSequences A reference to a boolean flag indicating whether the user terminal can handle VT sequences.
    */
    void enableVTSequences(bool& handleVTSequences) {
      HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
      if (hOut == INVALID_HANDLE_VALUE)
      {
        std::cerr << "Couldn't get the console handle. Quitting.\n" << std::endl;
        exit(EXIT_FAILURE);
      }

      DWORD dwMode = 0;
      if (!GetConsoleMode(hOut, &dwMode))
      {
        std::cerr << "Unable to enter VT processing mode. Quitting.\n" << std::endl;
        exit(EXIT_FAILURE);
      }

      dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
      if (!SetConsoleMode(hOut, dwMode))
      {
        std::cerr << "Unable to enter VT processing mode. Quitting.\n" << std::endl;
        exit(EXIT_FAILURE);
      }
      handleVTSequences = true;
    }
    #else //If not on Windows, runs a simplifed function just for code reusability

    void enableVTSequences(bool& handleVTSequences) {
      handleVTSequences = true;
    }
    #endif

    /**
     * @brief Generates an ANSI escape code for the background or for the text based on the position
     * 
     * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color applies.
     * @tparam red Red component of the color (0-255).
     * @tparam green Green component of the color (0-255).
     * @tparam blue Blue component of the color (0-255).
     * @return The ANSI escape code for the specified color either for the background or for the text.
    */
    template <uint8_t position, uint8_t red, uint8_t green, uint8_t blue>
    string getColor(){
      if (position == 1){ // TEXT
        return "\033[38;2;" + to_string(red) + ";" + to_string(green) + ";" + to_string(blue) + "m";
      }
      else { // BACKGROUND
        return "\033[48;2;" + to_string(red) + ";" + to_string(green) + ";" + to_string(blue) + "m";
      }
    }

    /**
     * @brief Applies a color to the given text based on the templates parameters
     * 
     * @tparam red Red component of the color (0-255).
     * @tparam green Green component of the color (0-255).
     * @tparam blue Blue component of the color (0-255).
     * 
     * @param text The text to color.
     * 
     * @return The colored text followed by the reset style.
    */
    template <uint8_t red, uint8_t green, uint8_t blue>
    string colorText(const string& text) {
      const string color = getColor<1, red, green, blue>();
      return color + text + RESET_STYLE;
    }
    
    /**
     * @brief Returns a color based on the templates parameters.
     * 
     * @tparam red Red component of the color (0-255).
     * @tparam green Green component of the color (0-255).
     * @tparam blue Blue component of the color (0-255).
     * @return The color calculated from the temlate parameters.
    */
    template <uint8_t red, uint8_t green, uint8_t blue>
    string colorText() {
      const string color = getColor<1, red, green, blue>();
      return color;
    }

    /**
     * @brief Applies a color for the background based on the templates parameters
     * 
     * @tparam red Red component of the color (0-255).
     * @tparam green Green component of the color (0-255).
     * @tparam blue Blue component of the color (0-255).
     * 
     * @param text The text to color.
     * 
     * @return The colored background followed by the reset style.
    */
    template <uint8_t red, uint8_t green, uint8_t blue>
    string colorBackground(const string& text) {
      const string color = getColor<0, red, green, blue>();
      return color + text + RESET_STYLE;
    }

    /**
     * @brief Returns a color for the background based on the templates parameters.
     * 
     * @tparam red Red component of the color (0-255).
     * @tparam green Green component of the color (0-255).
     * @tparam blue Blue component of the color (0-255).
     * 
     * @param text The text to color.
     * 
     * @return The colored text followed by the reset style.
    */
    template <uint8_t red, uint8_t green, uint8_t blue>
    string colorBackground() {
      const string color = getColor<0, red, green, blue>();
      return color;
    }

    /**
     * @brief Check if a given position is equal to TEXT (1) or to BACKGROUND (0), otherwise throws an error
    */
    void checkPosition(uint8_t position){
      if (position != 1 && position != 0){
        std::cerr << "Position must be either 0 or 1";
        exit(EXIT_FAILURE);
      }
    }
  }

  constexpr uint8_t TEXT = 1;
  constexpr uint8_t BACKGROUND = 0;

  /**
   * @brief Returns the stream with the a color, specified from the template params, for the background or text.
   * 
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color applies.
   * @tparam red Red component of the color (0-255).
   * @tparam green Green component of the color (0-255).
   * @tparam blue Blue component of the color (0-255).
   * 
   * @param os The stream to apply the color to.
   * 
   * @return The modified stream.
  */
  template <uint8_t position, uint8_t red, uint8_t green, uint8_t blue>
  ostream& color(ostream& os){
    _private::checkPosition(position);
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    os << _private::getColor<position, red, green, blue>();
    return os;
  }

  /**
   * @brief Applies the specified color, specified from the template params, to the text or background.
   * 
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color applies.
   * @tparam red Red component of the color (0-255).
   * @tparam green Green component of the color (0-255).
   * @tparam blue Blue component of the color (0-255).
   * 
   * @param text The text to color.
   * 
   * @return The modified text with the applied color.
  */
  template <uint8_t position, uint8_t red, uint8_t green, uint8_t blue>
  string color(const string& text) {
    _private::checkPosition(position);
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    return position == TEXT ? _private::colorText<red, green, blue>(text) : _private::colorBackground<red, green, blue>(text);
  }
  
  /**
   * @brief Applies the specified color, specified from the template params, to the text.
   * 
   * @tparam red Red component of the color (0-255).
   * @tparam green Green component of the color (0-255).
   * @tparam blue Blue component of the color (0-255).
   * 
   * @param text The text to color.
   * 
   * @return The modified text with the applied color.
  */
  template<uint8_t red, uint8_t green, uint8_t blue>
  string color(const string& text) {
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    return _private::colorText<red, green, blue>(text);
  }

  /**
   * @brief Applies to the stream the color specified from the template params.
   * 
   * @tparam red Red component of the color (0-255).
   * @tparam green Green component of the color (0-255).
   * @tparam blue Blue component of the color (0-255).
   * 
   * @param os The stream to apply the color to.
   * 
   * @return The modified stream.
  */
  template<uint8_t red, uint8_t green, uint8_t blue>
  ostream& color(ostream& os) {
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    os <<  _private::colorBackground<red, green, blue>();
    return os;
  }

  /**
   * @brief Applies the specified color, specified from the template params, to the background.
   * 
   * @tparam red Red component of the color (0-255).
   * @tparam green Green component of the color (0-255).
   * @tparam blue Blue component of the color (0-255).
   * 
   * @param text The text to color.
   * 
   * @return The modified text with the applied background color.
  */
  template<uint8_t red, uint8_t green, uint8_t blue>
  string on_color(const string& text) {
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    return _private::colorText<red, green, blue>(text);
  }

  /**
   * @brief Applies to the stream the color specified from the template params.
   * 
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color applies.
   * @tparam red Red component of the color (0-255).
   * @tparam green Green component of the color (0-255).
   * @tparam blue Blue component of the color (0-255).
   * 
   * @param os The stream to apply the color to.
   * 
   * @return The modified stream.
  */
  template<uint8_t red, uint8_t green, uint8_t blue>
  ostream& on_color(ostream& os) {
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    os <<  _private::colorBackground<red, green, blue>();
    return os;
  }

  // Functions for grey color

  /**
   * @brief Applies the color grey either for the background or for the text, based on the position
   * 
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color applies.
   * 
   * @param os The stream to apply the color to.
   * 
   * @return The modified stream.
  */
  template<uint8_t position>
  ostream& grey(ostream& os){
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    _private::checkPosition(position);
    os << ((position == TEXT) ? _private::color_text["grey"] : _private::color_background["grey"]);
    return os;
  }

  /**
   * @brief Applies the color grey either for the background or for the text, based on the position
   * 
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color applies.
   * 
   * @param text The text to color.
   * 
   * @return The modified text with the grey color.
  */

  template<uint8_t position>
  string grey(const string& text) {
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    _private::checkPosition(position);
    const string grey = (position == TEXT) ? _private::color_text["grey"] : _private::color_background["grey"];
    return grey + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the color grey to the text
   * 
   * @param text The text to color.
   * 
   * @return The modified text with the grey color.
  */
  string grey(const string& text) {
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    const string grey = _private::color_text["grey"];
    return grey + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the color grey to the text
   * 
   * @param os The stream to apply the color to.
   * 
   * @return The modified stream.
  */
  ostream& grey(ostream& os){
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    os << _private::color_text["grey"];
    return os;
  }

  /**
   * @brief Applies the color grey to the background
   * 
   * @param text The text to color.
   * 
   * @return The modified text with the grey color.
  */
  string on_grey(const string& text){
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    const string grey = _private::color_background["grey"];
    return grey + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the color grey to the background
   * 
   * @param os The stream to apply the color to.
   * 
   * @return The modified stream.
  */
  ostream& on_grey(ostream& os){
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    os << _private::color_background["grey"];
    return os;
  }

  // Functions for bright grey color
  /**
   * @brief Applies a bright grey color to the text or background based on the position.
   *
   * This function applies the bright grey color to the specified position (either text or background).
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param os The output stream to which the color will be applied.
   * @return The modified output stream with the bright grey color applied.
  */
  template<uint8_t position>
  ostream& bright_grey(ostream& os){
    _private::checkPosition(position);
    os << ((position == TEXT)? _private::color_text["bright grey"] : _private::color_background["bright grey"]);
    return os;
  }

  /**
   * @brief Applies a bright grey color to the text based on the position.
   *
   * This function applies the bright grey color to the specified position (either text or background).
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param text The text to which the bright grey color will be applied.
   * @return The modified text with the bright grey color applied.
  */
  template<uint8_t position>
  string bright_grey(const string& text) {
    _private::checkPosition(position);
    const string bright_grey = (position == TEXT)? _private::color_text["bright grey"] : _private::color_background["bright grey"];
    return bright_grey + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies a bright grey color to the text.
   *
   * This function applies the bright grey color to the text.
   *
   * @param text The text to which the bright grey color will be applied.
   * @return The modified text with the bright grey color applied.
  */
  string bright_grey(const string& text) {
    const string bright_grey = _private::color_text["bright grey"];
    return bright_grey + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies a bright grey color to the text.
   *
   * This function applies the bright grey color to the text.
   *
   * @param os The output stream to which the bright grey color will be applied.
   * @return The modified output stream with the bright grey color applied.
  */
  ostream& bright_grey(ostream& os){
    os << _private::color_text["bright grey"];
    return os;
  }

  /**
   * @brief Applies a bright grey color to the background.
   *
   * This function applies the bright grey color to the background.
   *
   * @param text The text to which the bright grey color will be applied.
   * @return The modified text with the bright grey color applied.
  */
  string on_bright_grey(const string& text){
    const string bright_grey = _private::color_background["bright grey"];
    return bright_grey + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies a bright grey color to the background.
   *
   * This function applies the bright grey color to the background.
   *
   * @param os The output stream to which the bright grey color will be applied.
   * @return The modified output stream with the bright grey color applied.
  */
  ostream& on_bright_grey(ostream& os){
    os << _private::color_background["bright grey"];
    return os;
  }

  // Functions for red color

  /**
   * @brief Applies the red color to the text or background based on the position.
   *
   * This function applies the red color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param os The output stream to which the color will be applied.
   * @return The modified output stream with the red color applied.
  */
  template<uint8_t position>
  ostream& red(ostream& os){
    _private::checkPosition(position);
    os << ((position == TEXT)? _private::color_text["red"] : _private::color_background["red"]);
    return os;
  }

  /**
   * @brief Applies the red color to the text based on the position.
   *
   * This function applies the red color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param text The text to which the red color will be applied.
   * @return The modified text with the red color applied.
  */
  template<uint8_t position>
  string red(const string& text) {
    _private::checkPosition(position);
    const string red = (position == TEXT)? _private::color_text["red"] : _private::color_background["red"];
    return red + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the red color to the text.
   *
   * This function applies the red color to the text.
   *
   * @param text The text to which the red color will be applied.
   * @return The modified text with the red color applied.
  */
  string red(const string& text) {
    const string red = _private::color_text["red"];
    return red + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the red color to the text.
   *
   * This function applies the red color to the text.
   *
   * @param os The output stream to which the red color will be applied.
   * @return The modified output stream with the red color applied.
  */
  ostream& red(ostream& os){
    os << _private::color_text["red"];
    return os;
  }

  /**
   * @brief Applies the red color to the background.
   *
   * This function applies the red color to the background.
   *
   * @param text The text to which the red color will be applied.
   * @return The modified text with the red color applied.
  */
  string on_red(const string& text){
    const string red = _private::color_background["red"];
    return red + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the red color to the background.
   *
   * This function applies the red color to the background.
   *
   * @param os The output stream to which the red color will be applied.
   * @return The modified output stream with the red color applied.
  */
  ostream& on_red(ostream& os){
    os << _private::color_background["red"];
    return os;
  }

  // Functions for bright red color

  /**
   * @brief Applies the bright red color to the text or background based on the position.
   *
   * This function applies the bright red color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param os The output stream to which the color will be applied.
   * @return The modified output stream with the bright red color applied.
  */
  template<uint8_t position>
  ostream& bright_red(ostream& os){
    _private::checkPosition(position);
    os << ((position == TEXT)? _private::color_text["bright red"] : _private::color_background["bright red"]);
    return os;
  }

  /**
   * @brief Applies the bright red color to the text based on the position.
   *
   * This function applies the bright red color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param text The text to which the bright red color will be applied.
   * @return The modified text with the bright red color applied.
  */
  template<uint8_t position>
  string bright_red(const string& text) {
    _private::checkPosition(position);
    const string bright_red = (position == TEXT)? _private::color_text["bright red"] : _private::color_background["bright red"];
    return bright_red + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright red color to the text.
   *
   * This function applies the bright red color to the text.
   *
   * @param text The text to which the bright red color will be applied.
   * @return The modified text with the bright red color applied.
  */
  string bright_red(const string& text) {
    const string bright_red = _private::color_text["bright red"];
    return bright_red + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright red color to the text.
   *
   * This function applies the bright red color to the text.
   *
   * @param os The output stream to which the bright red color will be applied.
   * @return The modified output stream with the bright red color applied.
  */
  ostream& bright_red(ostream& os){
    os << _private::color_text["bright red"];
    return os;
  }

  /**
   * @brief Applies the bright red color to the background.
   *
   * This function applies the bright red color to the background.
   *
   * @param text The text to which the bright red color will be applied.
   * @return The modified text with the bright red color applied.
  */
  string on_bright_red(const string& text){
    const string bright_red = _private::color_background["bright red"];
    return bright_red + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright red color to the background.
   *
   * This function applies the bright red color to the background.
   *
   * @param os The output stream to which the bright red color will be applied.
   * @return The modified output stream with the bright red color applied.
  */
  ostream& on_bright_red(ostream& os){
    os << _private::color_background["bright red"];
    return os;
  }

  //Functions for the color green

  /**
   * @brief Applies the green color to the text or background based on the position.
   *
   * This function applies the green color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param os The output stream to which the color will be applied.
   * @return The modified output stream with the green color applied.
  */
  template<uint8_t position>
  ostream& green(ostream& os){
    _private::checkPosition(position);
    os << ((position == TEXT)? _private::color_text["green"] : _private::color_background["green"]);
    return os;
  }

  /**
   * @brief Applies the green color to the text based on the position.
   *
   * This function applies the green color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param text The text to which the green color will be applied.
   * @return The modified text with the green color applied.
  */
  template<uint8_t position>
  string green(const string& text) {
    _private::checkPosition(position);
    const string green = (position == TEXT)? _private::color_text["green"] : _private::color_background["green"];
    return green + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the green color to the text.
   *
   * This function applies the green color to the text.
   *
   * @param text The text to which the green color will be applied.
   * @return The modified text with the green color applied.
  */
  string green(const string& text) {
    const string green = _private::color_text["green"];
    return green + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the green color to the text.
   *
   * This function applies the green color to the text.
   *
   * @param os The output stream to which the green color will be applied.
   * @return The modified output stream with the green color applied.
  */
  ostream& green(ostream& os){
    os << _private::color_text["green"];
    return os;
  }

  /**
   * @brief Applies the green color to the background.
   *
   * This function applies the green color to the background.
   *
   * @param text The text to which the green color will be applied.
   * @return The modified text with the green color applied.
  */
  string on_green(const string& text){
    const string green = _private::color_background["green"];
    return green + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the green color to the background.
   *
   * This function applies the green color to the background.
   *
   * @param os The output stream to which the green color will be applied.
   * @return The modified output stream with the green color applied.
  */
  ostream& on_green(ostream& os){
    os << _private::color_background["green"];
    return os;
  }

  // Functions for bright green color
  /**
   * @brief Applies the bright green color to the text or background based on the position.
   *
   * This function applies the bright green color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param os The output stream to which the color will be applied.
   * @return The modified output stream with the bright green color applied.
  */
  template<uint8_t position>
  ostream& bright_green(ostream& os){
    _private::checkPosition(position);
    os << ((position == TEXT)? _private::color_text["bright green"] : _private::color_background["bright green"]);
    return os;
  }

  /**
   * @brief Applies the bright green color to the text based on the position.
   *
   * This function applies the bright green color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param text The text to which the bright green color will be applied.
   * @return The modified text with the bright green color applied.
  */
  template<uint8_t position>
  string bright_green(const string& text) {
    _private::checkPosition(position);
    const string bright_green = (position == TEXT)? _private::color_text["bright green"] : _private::color_background["bright green"];
    return bright_green + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright green color to the text.
   *
   * This function applies the bright green color to the text.
   *
   * @param text The text to which the bright green color will be applied.
   * @return The modified text with the bright green color applied.
  */
  string bright_green(const string& text) {
    const string bright_green = _private::color_text["bright green"];
    return bright_green + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright green color to the text.
   *
   * This function applies the bright green color to the text.
   *
   * @param os The output stream to which the bright green color will be applied.
   * @return The modified output stream with the bright green color applied.
  */
  ostream& bright_green(ostream& os){
    os << _private::color_text["bright green"];
    return os;
  }

  /**
   * @brief Applies the bright green color to the background.
   *
   * This function applies the bright green color to the background.
   *
   * @param text The text to which the bright green color will be applied.
   * @return The modified text with the bright green color applied.
  */
  string on_bright_green(const string& text){
    const string bright_green = _private::color_background["bright green"];
    return bright_green + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright green color to the background.
   *
   * This function applies the bright green color to the background.
   *
   * @param os The output stream to which the bright green color will be applied.
   * @return The modified output stream with the bright green color applied.
  */
  ostream& on_bright_green(ostream& os){
    os << _private::color_background["bright green"];
    return os;
  }

  // Functions for yellow color
  /**
   * @brief Applies the yellow color to the text or background based on the position.
   *
   * This function applies the yellow color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param os The output stream to which the color will be applied.
   * @return The modified output stream with the yellow color applied.
  */
  template<uint8_t position>
  ostream& yellow(ostream& os){
    _private::checkPosition(position);
    os << ((position == TEXT)? _private::color_text["yellow"] : _private::color_background["yellow"]);
    return os;
  }

  /**
   * @brief Applies the yellow color to the text based on the position.
   *
   * This function applies the yellow color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param text The text to which the yellow color will be applied.
   * @return The modified text with the yellow color applied.
   */
  template<uint8_t position>
  string yellow(const string& text) {
    _private::checkPosition(position);
    const string yellow = (position == TEXT)? _private::color_text["yellow"] : _private::color_background["yellow"];
    return yellow + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the yellow color to the text.
   *
   * This function applies the yellow color to the text.
   *
   * @param text The text to which the yellow color will be applied.
   * @return The modified text with the yellow color applied.
   */
  string yellow(const string& text) {
    const string yellow = _private::color_text["yellow"];
    return yellow + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the yellow color to the text.
   *
   * This function applies the yellow color to the text.
   *
   * @param os The output stream to which the yellow color will be applied.
   * @return The modified output stream with the yellow color applied.
   */
  ostream& yellow(ostream& os){
    os << _private::color_text["yellow"];
    return os;
  }

  /**
   * @brief Applies the yellow color to the background.
   *
   * This function applies the yellow color to the background.
   *
   * @param text The text to which the yellow color will be applied.
   * @return The modified text with the yellow color applied.
   */
  string on_yellow(const string& text){
    const string yellow = _private::color_background["yellow"];
    return yellow + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the yellow color to the background.
   *
   * This function applies the yellow color to the background.
   *
   * @param os The output stream to which the yellow color will be applied.
   * @return The modified output stream with the yellow color applied.
   */
  ostream& on_yellow(ostream& os){
    os << _private::color_background["yellow"];
    return os;
  }

  // Functions for bright yellow color
  /**
   * @brief Applies the bright yellow color to the text or background based on the position.
   *
   * This function applies the bright yellow color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param os The output stream to which the color will be applied.
   * @return The modified output stream with the bright yellow color applied.
  */
  template<uint8_t position>
  ostream& bright_yellow(ostream& os){
    _private::checkPosition(position);
    os << ((position == TEXT)? _private::color_text["bright yellow"] : _private::color_background["bright yellow"]);
    return os;
  }

  /**
   * @brief Applies the bright yellow color to the text based on the position.
   *
   * This function applies the bright yellow color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param text The text to which the bright yellow color will be applied.
   * @return The modified text with the bright yellow color applied.
  */
  template<uint8_t position>
  string bright_yellow(const string& text) {
    _private::checkPosition(position);
    const string bright_yellow = (position == TEXT)? _private::color_text["bright yellow"] : _private::color_background["bright yellow"];
    return bright_yellow + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright yellow color to the text.
   *
   * This function applies the bright yellow color to the text.
   *
   * @param text The text to which the bright yellow color will be applied.
   * @return The modified text with the bright yellow color applied.
  */
  string bright_yellow(const string& text) {
    const string bright_yellow = _private::color_text["bright yellow"];
    return bright_yellow + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright yellow color to the text.
   *
   * This function applies the bright yellow color to the text.
   *
   * @param os The output stream to which the bright yellow color will be applied.
   * @return The modified output stream with the bright yellow color applied.
  */
  ostream& bright_yellow(ostream& os){
    os << _private::color_text["bright yellow"];
    return os;
  }

  /**
   * @brief Applies the bright yellow color to the background.
   *
   * This function applies the bright yellow color to the background.
   *
   * @param text The text to which the bright yellow color will be applied.
   * @return The modified text with the bright yellow color applied.
  */
  string on_bright_yellow(const string& text){
    const string bright_yellow = _private::color_background["bright yellow"];
    return bright_yellow + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright yellow color to the background.
   *
   * This function applies the bright yellow color to the background.
   *
   * @param os The output stream to which the bright yellow color will be applied.
   * @return The modified output stream with the bright yellow color applied.
  */
  ostream& on_bright_yellow(ostream& os){
    os << _private::color_background["bright yellow"];
    return os;
  }

  // Functions for blue color
  /**
   * @brief Applies the blue color to the text or background based on the position.
   *
   * This function applies the blue color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param os The output stream to which the color will be applied.
   * @return The modified output stream with the blue color applied.
  */
  template<uint8_t position>
  ostream& blue(ostream& os){
    _private::checkPosition(position);
    os << ((position == TEXT)? _private::color_text["blue"] : _private::color_background["blue"]);
    return os;
  }

  /**
   * @brief Applies the blue color to the text based on the position.
   *
   * This function applies the blue color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param text The text to which the blue color will be applied.
   * @return The modified text with the blue color applied.
  */
  template<uint8_t position>
  string blue(const string& text) {
    _private::checkPosition(position);
    const string blue = (position == TEXT)? _private::color_text["blue"] : _private::color_background["blue"];
    return blue + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the blue color to the text.
   *
   * This function applies the blue color to the text.
   *
   * @param text The text to which the blue color will be applied.
   * @return The modified text with the blue color applied.
  */
  string blue(const string& text) {
    const string blue = _private::color_text["blue"];
    return blue + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the blue color to the text.
   *
   * This function applies the blue color to the text.
   *
   * @param os The output stream to which the blue color will be applied.
   * @return The modified output stream with the blue color applied.
  */
  ostream& blue(ostream& os){
    os << _private::color_text["blue"];
    return os;
  }

  /**
   * @brief Applies the blue color to the background.
   *
   * This function applies the blue color to the background.
   *
   * @param text The text to which the blue color will be applied.
   * @return The modified text with the blue color applied.
  */
  string on_blue(const string& text){
    const string blue = _private::color_background["blue"];
    return blue + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the blue color to the background.
   *
   * This function applies the blue color to the background.
   *
   * @param os The output stream to which the blue color will be applied.
   * @return The modified output stream with the blue color applied.
  */
  ostream& on_blue(ostream& os){
    os << _private::color_background["blue"];
    return os;
  }

  // Functions for bright blue color
  /**
   * @brief Applies the bright blue color to the text or background based on the position.
   *
   * This function applies the bright blue color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param os The output stream to which the color will be applied.
   * @return The modified output stream with the bright blue color applied.
  */
  template<uint8_t position>
  ostream& bright_blue(ostream& os){
    _private::checkPosition(position);
    os << ((position == TEXT)? _private::color_text["bright blue"] : _private::color_background["bright blue"]);
    return os;
  }

  /**
   * @brief Applies the bright blue color to the text based on the position.
   *
   * This function applies the bright blue color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param text The text to which the bright blue color will be applied.
   * @return The modified text with the bright blue color applied.
  */
  template<uint8_t position>
  string bright_blue(const string& text) {
    _private::checkPosition(position);
    const string bright_blue = (position == TEXT)? _private::color_text["bright blue"] : _private::color_background["bright blue"];
    return bright_blue + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright blue color to the text.
   *
   * This function applies the bright blue color to the text.
   *
   * @param text The text to which the bright blue color will be applied.
   * @return The modified text with the bright blue color applied.
  */
  string bright_blue(const string& text) {
    const string bright_blue = _private::color_text["bright blue"];
    return bright_blue + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright blue color to the text.
   *
   * This function applies the bright blue color to the text.
   *
   * @param os The output stream to which the bright blue color will be applied.
   * @return The modified output stream with the bright blue color applied.
  */
  ostream& bright_blue(ostream& os){
    os << _private::color_text["bright blue"];
    return os;
  }

  /**
   * @brief Applies the bright blue color to the background.
   *
   * This function applies the bright blue color to the background.
   *
   * @param text The text to which the bright blue color will be applied.
   * @return The modified text with the bright blue color applied.
  */
  string on_bright_blue(const string& text){
    const string bright_blue = _private::color_background["bright blue"];
    return bright_blue + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright blue color to the background.
   *
   * This function applies the bright blue color to the background.
   *
   * @param os The output stream to which the bright blue color will be applied.
   * @return The modified output stream with the bright blue color applied.
  */
  ostream& on_bright_blue(ostream& os){
    os << _private::color_background["bright blue"];
    return os;
  }

  // Functions for magenta color
  /**
   * @brief Applies the magenta color to the text or background based on the position.
   *
   * This function applies the magenta color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param os The output stream to which the color will be applied.
   * @return The modified output stream with the magenta color applied.
  */
  template<uint8_t position>
  ostream& magenta(ostream& os){
    _private::checkPosition(position);
    os << ((position == TEXT)? _private::color_text["magenta"] : _private::color_background["magenta"]);
    return os;
  }

  /**
   * @brief Applies the magenta color to the text based on the position.
   *
   * This function applies the magenta color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param text The text to which the magenta color will be applied.
   * @return The modified text with the magenta color applied.
  */
  template<uint8_t position>
  string magenta(const string& text) {
    _private::checkPosition(position);
    const string magenta = (position == TEXT)? _private::color_text["magenta"] : _private::color_background["magenta"];
    return magenta + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the magenta color to the text.
   *
   * This function applies the magenta color to the text.
   *
   * @param text The text to which the magenta color will be applied.
   * @return The modified text with the magenta color applied.
  */
  string magenta(const string& text) {
    const string magenta = _private::color_text["magenta"];
    return magenta + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the magenta color to the text.
   *
   * This function applies the magenta color to the text.
   *
   * @param os The output stream to which the magenta color will be applied.
   * @return The modified output stream with the magenta color applied.
  */
  ostream& magenta(ostream& os){
    os << _private::color_text["magenta"];
    return os;
  }

  /**
   * @brief Applies the magenta color to the background.
   *
   * This function applies the magenta color to the background.
   *
   * @param text The text to which the magenta color will be applied.
   * @return The modified text with the magenta color applied.
  */
  string on_magenta(const string& text){
    const string magenta = _private::color_background["magenta"];
    return magenta + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the magenta color to the background.
   *
   * This function applies the magenta color to the background.
   *
   * @param os The output stream to which the magenta color will be applied.
   * @return The modified output stream with the magenta color applied.
  */
  ostream& on_magenta(ostream& os){
    os << _private::color_background["magenta"];
    return os;
  }

  // Functions for bright magenta color
  /**
   * @brief Applies the bright magenta color to the text or background based on the position.
   *
   * This function applies the bright magenta color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param os The output stream to which the color will be applied.
   * @return The modified output stream with the bright magenta color applied.
  */
  template<uint8_t position>
  ostream& bright_magenta(ostream& os){
    _private::checkPosition(position);
    os << ((position == TEXT)? _private::color_text["bright magenta"] : _private::color_background["bright magenta"]);
    return os;
  }

  /**
   * @brief Applies the bright magenta color to the text based on the position.
   *
   * This function applies the bright magenta color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param text The text to which the bright magenta color will be applied.
   * @return The modified text with the bright magenta color applied.
  */
  template<uint8_t position>
  string bright_magenta(const string& text) {
    _private::checkPosition(position);
    const string bright_magenta = (position == TEXT)? _private::color_text["bright magenta"] : _private::color_background["bright magenta"];
    return bright_magenta + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright magenta color to the text.
   *
   * This function applies the bright magenta color to the text.
   *
   * @param text The text to which the bright magenta color will be applied.
   * @return The modified text with the bright magenta color applied.
  */
  string bright_magenta(const string& text) {
    const string bright_magenta = _private::color_text["bright magenta"];
    return bright_magenta + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright magenta color to the text.
   *
   * This function applies the bright magenta color to the text.
   *
   * @param os The output stream to which the bright magenta color will be applied.
   * @return The modified output stream with the bright magenta color applied.
  */
  ostream& bright_magenta(ostream& os){
    os << _private::color_text["bright magenta"];
    return os;
  }

  /**
   * @brief Applies the bright magenta color to the background.
   *
   * This function applies the bright magenta color to the background.
   *
   * @param text The text to which the bright magenta color will be applied.
   * @return The modified text with the bright magenta color applied.
  */
  string on_bright_magenta(const string& text){
    const string bright_magenta = _private::color_background["bright magenta"];
    return bright_magenta + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright magenta color to the background.
   *
   * This function applies the bright magenta color to the background.
   *
   * @param os The output stream to which the bright magenta color will be applied.
   * @return The modified output stream with the bright magenta color applied.
  */
  ostream& on_bright_magenta(ostream& os){
    os << _private::color_background["bright magenta"];
    return os;
  }

  // Functions for cyan color
  /**
   * @brief Applies the cyan color to the text or background based on the position.
   *
   * This function applies the cyan color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param os The output stream to which the color will be applied.
   * @return The modified output stream with the cyan color applied.
  */
  template<uint8_t position>
  ostream& cyan(ostream& os){
    _private::checkPosition(position);
    os << ((position == TEXT)? _private::color_text["cyan"] : _private::color_background["cyan"]);
    return os;
  }

  /**
   * @brief Applies the cyan color to the text based on the position.
   *
   * This function applies the cyan color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param text The text to which the cyan color will be applied.
   * @return The modified text with the cyan color applied.
  */
  template<uint8_t position>
  string cyan(const string& text) {
    _private::checkPosition(position);
    const string cyan = (position == TEXT)? _private::color_text["cyan"] : _private::color_background["cyan"];
    return cyan + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the cyan color to the text.
   *
   * This function applies the cyan color to the text.
   *
   * @param text The text to which the cyan color will be applied.
   * @return The modified text with the cyan color applied.
  */
  string cyan(const string& text) {
    const string cyan = _private::color_text["cyan"];
    return cyan + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the cyan color to the text.
   *
   * This function applies the cyan color to the text.
   *
   * @param os The output stream to which the cyan color will be applied.
   * @return The modified output stream with the cyan color applied.
  */
  ostream& cyan(ostream& os){
    os << _private::color_text["cyan"];
    return os;
  }

  /**
   * @brief Applies the cyan color to the background.
   *
   * This function applies the cyan color to the background.
   *
   * @param text The text to which the cyan color will be applied.
   * @return The modified text with the cyan color applied.
  */
  string on_cyan(const string& text){
    const string cyan = _private::color_background["cyan"];
    return cyan + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the cyan color to the background.
   *
   * This function applies the cyan color to the background.
   *
   * @param os The output stream to which the cyan color will be applied.
   * @return The modified output stream with the cyan color applied.
  */
  ostream& on_cyan(ostream& os){
    os << _private::color_background["cyan"];
    return os;
  }

  //Functions for bright cyan color

  /**
   * @brief Applies the bright cyan color to the text or background based on the position.
   *
   * This function applies the bright cyan color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param os The output stream to which the color will be applied.
   * @return The modified output stream with the bright cyan color applied.
   */
  template<uint8_t position>
  ostream& bright_cyan(ostream& os){
    _private::checkPosition(position);
    os << ((position == TEXT)? _private::color_text["bright cyan"] : _private::color_background["bright cyan"]);
    return os;
  }

  /**
   * @brief Applies the bright cyan color to the text based on the position.
   *
   * This function applies the bright cyan color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param text The text to which the bright cyan color will be applied.
   * @return The modified text with the bright cyan color applied.
   */
  template<uint8_t position>
  string bright_cyan(const string& text) {
    _private::checkPosition(position);
    const string bright_cyan = (position == TEXT)? _private::color_text["bright cyan"] : _private::color_background["bright cyan"];
    return bright_cyan + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright cyan color to the text.
   *
   * This function applies the bright cyan color to the text.
   *
   * @param text The text to which the bright cyan color will be applied.
   * @return The modified text with the bright cyan color applied.
   */
  string bright_cyan(const string& text) {
    const string bright_cyan = _private::color_text["bright cyan"];
    return bright_cyan + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright cyan color to the text.
   *
   * This function applies the bright cyan color to the text.
   *
   * @param os The output stream to which the bright cyan color will be applied.
   * @return The modified output stream with the bright cyan color applied.
   */
  ostream& bright_cyan(ostream& os){
    os << _private::color_text["bright cyan"];
    return os;
  }

  /**
   * @brief Applies the bright cyan color to the background.
   *
   * This function applies the bright cyan color to the background.
   *
   * @param text The text to which the bright cyan color will be applied.
   * @return The modified text with the bright cyan color applied.
   */
  string on_bright_cyan(const string& text){
    const string bright_cyan = _private::color_background["bright cyan"];
    return bright_cyan + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright cyan color to the background.
   *
   * This function applies the bright cyan color to the background.
   *
   * @param os The output stream to which the bright cyan color will be applied.
   * @return The modified output stream with the bright cyan color applied.
   */
  ostream& on_bright_cyan(ostream& os){
    os << _private::color_background["bright cyan"];
    return os;
  }

  // Functions for white color
  /**
   * @brief Applies the white color to the text or background based on the position.
   *
   * This function applies the white color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param os The output stream to which the color will be applied.
   * @return The modified output stream with the white color applied.
  */
  template<uint8_t position>
  ostream& white(ostream& os){
    _private::checkPosition(position);
    os << ((position == TEXT)? _private::color_text["white"] : _private::color_background["white"]);
    return os;
  }

  /**
   * @brief Applies the white color to the text based on the position.
   *
   * This function applies the white color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param text The text to which the white color will be applied.
   * @return The modified text with the white color applied.
  */
  template<uint8_t position>
  string white(const string& text) {
    _private::checkPosition(position);
    const string white = (position == TEXT)? _private::color_text["white"] : _private::color_background["white"];
    return white + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the white color to the text.
   *
   * This function applies the white color to the text.
   *
   * @param text The text to which the white color will be applied.
   * @return The modified text with the white color applied.
  */
  string white(const string& text) {
    const string white = _private::color_text["white"];
    return white + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the white color to the text.
   *
   * This function applies the white color to the text.
   *
   * @param os The output stream to which the white color will be applied.
   * @return The modified output stream with the white color applied.
  */
  ostream& white(ostream& os){
    os << _private::color_text["white"];
    return os;
  }

  /**
   * @brief Applies the white color to the background.
   *
   * This function applies the white color to the background.
   *
   * @param text The text to which the white color will be applied.
   * @return The modified text with the white color applied.
  */
  string on_white(const string& text){
    const string white = _private::color_background["white"];
    return white + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the white color to the background.
   *
   * This function applies the white color to the background.
   *
   * @param os The output stream to which the white color will be applied.
   * @return The modified output stream with the white color applied.
  */
  ostream& on_white(ostream& os){
    os << _private::color_background["white"];
    return os;
  }

  //Functions for bright white color

  /**
   * @brief Applies the bright white color to the text or background based on the position.
   *
   * This function applies the bright white color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param os The output stream to which the color will be applied.
   * @return The modified output stream with the bright white color applied.
   */
  template<uint8_t position>
  ostream& bright_white(ostream& os){
    _private::checkPosition(position);
    os << ((position == TEXT)? _private::color_text["bright white"] : _private::color_background["bright white"]);
    return os;
  }

  /**
   * @brief Applies the bright white color to the text based on the position.
   *
   * This function applies the bright white color to the specified position (either text or background),
   * depending on the value of the position parameter.
   *
   * @tparam position Either TEXT (1) or BACKGROUND (0), indicating where the color should be applied.
   * @param text The text to which the bright white color will be applied.
   * @return The modified text with the bright white color applied.
   */
  template<uint8_t position>
  string bright_white(const string& text) {
    _private::checkPosition(position);
    const string bright_white = (position == TEXT)? _private::color_text["bright white"] : _private::color_background["bright white"];
    return bright_white + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright white color to the text.
   *
   * This function applies the bright white color to the text.
   *
   * @param text The text to which the bright white color will be applied.
   * @return The modified text with the bright white color applied.
   */
  string bright_white(const string& text) {
    const string bright_white = _private::color_text["bright white"];
    return bright_white + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright white color to the text.
   *
   * This function applies the bright white color to the text.
   *
   * @param os The output stream to which the bright white color will be applied.
   * @return The modified output stream with the bright white color applied.
   */
  ostream& bright_white(ostream& os){
    os << _private::color_text["bright white"];
    return os;
  }

  /**
   * @brief Applies the bright white color to the background.
   *
   * This function applies the bright white color to the background.
   *
   * @param text The text to which the bright white color will be applied.
   * @return The modified text with the bright white color applied.
   */
  string on_bright_white(const string& text){
    const string bright_white = _private::color_background["bright white"];
    return bright_white + text + _private::RESET_STYLE;
  }

  /**
   * @brief Applies the bright white color to the background.
   *
   * This function applies the bright white color to the background.
   *
   * @param os The output stream to which the bright white color will be applied.
   * @return The modified output stream with the bright white color applied.
   */
  ostream& on_bright_white(ostream& os){
    os << _private::color_background["bright white"];
    return os;
  }

  //Functions for bold style

  /**
   * @brief Applies bold style to the stream.
   * 
   * @param os The stream to apply the bold style to.
   * 
   * @return The modified stream.
  */
  ostream& bold(ostream& os){
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    return os << "\033[1m";
  }
  
  /**
   * @brief Applies bold style to the text.
   * 
   * @param text The text to apply the bold style to.
   * 
   * @return The modified text with bold style applied.
  */
  string bold(const string& text) {
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    const string bold = "\033[1m";
    return bold + text + _private::RESET_STYLE;
  }

  //Functions for italic style

  /**
   * @brief Applies italic style to the stream.
   * 
   * @param os The stream to apply the italic style to.
   * 
   * @return The modified stream.
  */
  ostream& italic(ostream& os){
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    return os << "\033[3m";
  }

  /**
   * @brief Applies italic style to the text.
   * 
   * @param text The text to apply the italic style to.
   * 
   * @return The modified text with italic style applied.
  */
  string italic(const string& text) {
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    const string italic = "\033[3m";
    return italic + text + _private::RESET_STYLE;
  }

  //Functions for underline style

  /**
   * @brief Applies underline style to the stream.
   * 
   * @param os The stream to apply the underline style to.
   * 
   * @return The modified stream.
  */
  ostream& underline(ostream& os){
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    return os << "\033[4m";
  }

  /**
   * @brief Applies underline style to the text.
   * 
   * @param text The text to apply the underline style to.
   * 
   * @return The modified text with underline style applied.
  */
  string underline(const string& text) { 
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    const string underline = "\033[4m";
    return underline + text + _private::RESET_STYLE;
  }

  //Functions for reverse style

  /**
   * @brief Applies reverse style to the stream.
   * 
   * @param os The stream to apply the reverse style to.
   * 
   * @return The modified stream.
  */
  ostream& reverse(ostream& os){
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    return os << "\033[7m";
  }

  /**
   * @brief Applies reverse style to the text.
   * 
   * @param text The text to apply the reverse style to.
   * 
   * @return The modified text with reverse style applied.
  */
  string reverse(const string& text) { 
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    const string reverse = "\033[7m";
    return reverse + text + _private::RESET_STYLE;
  }

  //Functions for reset style

  /**
   * @brief Resets the stream to the default style.
   * 
   * @param os The stream to reset.
   * @return The modified stream with the default style.
  */
  ostream& reset(ostream& os){
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    os << _private::RESET_STYLE;
    return os;
  }

  /**
   * @brief Resets the text to the default style.
   * 
   * @param text The text to reset.
   * @return The modified text with the default style.
  */
  string reset(const string& text){
    if (!_private::handleVTSequences) _private::enableVTSequences(_private::handleVTSequences);
    return text + _private::RESET_STYLE;
  }
}

// int main() {

//   cout << CLIStyle::grey<1>("grey") << endl;
//   cout << CLIStyle::red<1>("red") << endl;
//   cout << CLIStyle::green<1>("green") << endl;
//   cout << CLIStyle::yellow<1>("yellow") << endl;
//   cout << CLIStyle::blue<1>("blue") << endl;
//   cout << CLIStyle::magenta<1>("magenta") << endl;
//   cout << CLIStyle::cyan<1>("cyan") << endl;
//   cout << CLIStyle::white<1>("white") << endl;

//   cout << CLIStyle::bright_red<1>("bright red") << endl;
//   cout << CLIStyle::bright_green<1>("bright green") << endl;
//   cout << CLIStyle::bright_yellow<1>("bright yellow") << endl;
//   cout << CLIStyle::bright_blue<1>("bright blue") << endl;
//   cout << CLIStyle::bright_magenta<1>("bright magenta") << endl;
//   cout << CLIStyle::bright_cyan<1>("bright cyan") << endl;
//   cout << CLIStyle::bright_white<1>("bright white") << endl;

//   cout << CLIStyle::grey<0>("grey") << endl;
//   cout << CLIStyle::red<0>("red") << endl;
//   cout << CLIStyle::green<0>("green") << endl;
//   cout << CLIStyle::yellow<0>("yellow") << endl;
//   cout << CLIStyle::blue<0>("blue") << endl;
//   cout << CLIStyle::magenta<0>("magenta") << endl;
//   cout << CLIStyle::cyan<0>("cyan") << endl;
//   cout << CLIStyle::white<0>("white") << endl;

//   cout << CLIStyle::bright_red<0>("bright red") << endl;
//   cout << CLIStyle::bright_green<0>("bright green") << endl;
//   cout << CLIStyle::bright_yellow<0>("bright yellow") << endl;
//   cout << CLIStyle::bright_blue<0>("bright blue") << endl;
//   cout << CLIStyle::bright_magenta<0>("bright magenta") << endl;
//   cout << CLIStyle::bright_cyan<0>("bright cyan") << endl;
//   cout << CLIStyle::bright_white<0>("bright white") << endl;

//   cout << CLIStyle::bold("bold") << endl;
//   cout << CLIStyle::underline("underline") << endl;
//   cout << CLIStyle::italic("italic") << endl;
//   cout << CLIStyle::reverse("reverse") << endl;

//   return 0;
// } 

//add 256 color codes