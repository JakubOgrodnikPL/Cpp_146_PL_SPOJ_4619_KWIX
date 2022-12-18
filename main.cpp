#include <iostream>
#include <string>
#include <deque>

enum class TagType {
	SelfClosing,
	Opening,
	Closing
};

struct TagProperty {
	std::string name;
	std::string value;
};

struct Tag {
	std::string name;
	std::deque<TagProperty> properties;
	TagType type;
};

char readNextChar();
void parseTag(Tag&);
void printTagProperties(Tag const&, int);
void printTag(Tag const&, int);

int main() {
	std::deque<Tag> tags;

	// Parse input
	while(true) {
		char c;
		while((c = readNextChar()) != '<' && c != EOF) {}
		if(c == EOF) { break; }
		Tag tag;
		parseTag(tag);
		tags.push_back(std::move(tag));
	}

	// Print output
	int indentation = 0;
	for(Tag const& tag: tags) {
		if(tag.type != TagType::Closing) {
			printTag(tag, indentation);
			printTagProperties(tag, indentation);
		}

		if(tag.type == TagType::Closing) {
			indentation -= 4;
		} else if(tag.type == TagType::Opening) {
			indentation += 4;
		}
	}

    return 0;
}

char readNextChar() {
	return std::cin.get();
}

bool isAlphanumeric(char c) {
	return (48 <= c && c <= 57) || (65 <= c && c <= 90) || (97 <= c && c <= 122);
}

enum class State {
	TagOpen,
	TagName,
	TagNameEnd,
	TagClose,
	PropertyName,
	PropertyNameEnd,
	PropertyValue,
	PropertyValueEnd
};

void parseTag(Tag& tag) {
	tag.type = TagType::Opening;
	State state = State::TagOpen;

	char c;
	while((c = readNextChar()) != '>') {
		if(state == State::TagOpen) {
			if(c == '/') {
				tag.type = TagType::Closing;
			} else {
				state = State::TagName;
				tag.name += c;
			}

		} else if(state == State::TagName) {
			if(isAlphanumeric(c)) {
				tag.name += c;
			} else {
				if(c == '/') {
					tag.type = TagType::SelfClosing;
					state = State::TagClose;
				} else {
					state = State::TagNameEnd;
				}
			}
		} else if(state == State::TagNameEnd) {
			if(c == '/') {
				tag.type = TagType::SelfClosing;
				state = State::TagClose;
			} else if(isAlphanumeric(c)) {
				tag.properties.emplace_back();
				tag.properties.back().name += c;
				state = State::PropertyName;
			}
		} else if(state == State::PropertyName) {
			if(isAlphanumeric(c)) {
				tag.properties.back().name += c;
			} else {
				state = State::PropertyNameEnd;
			}
		} else if(state == State::PropertyNameEnd) {
			if(c == '"') {
				state = State::PropertyValue;
			}
		} else if(state == State::PropertyValue) {
			if(c == '"') {
				state = State::PropertyValueEnd;
			} else {
				tag.properties.back().value += c;
			}
		} else if(state == State::PropertyValueEnd) {
			if(c == '/') {
				tag.type = TagType::SelfClosing;
				state = State::TagClose;
			} else if(isAlphanumeric(c)) {
				tag.properties.emplace_back();
				tag.properties.back().name += c;
				state = State::PropertyName;
			}
		}
	}
}

void printTag(Tag const& tag, int indentation) {
	std::cout << std::string(indentation, ' ') << tag.name << ":\n";
}

void printTagProperties(Tag const& tag, int indentation) {
	for(TagProperty const& prop: tag.properties) {
		std::cout << std::string(indentation + 4, ' ') << prop.name << " = " << prop.value << "\n";
	}
}
