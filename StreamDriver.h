#ifndef STREAMDRIVER_H__
#define STREAMDRIVER_H__

#include <stdlib.h>

#ifndef STREAMDRIVER_COMMAND_BUFFER_SIZE
#define STREAMDRIVER_COMMAND_BUFFER_SIZE 512
#endif

class StreamDriver {
public:
	typedef void (*CommandHandler)(StreamDriver* driver, const char* cmd, size_t cmdLen);

	struct Command {
		char commandChar;
		CommandHandler handler;
		const char* description;
	};

	const Stream& stream;

private:
	char commandBuffer[STREAMDRIVER_COMMAND_BUFFER_SIZE];
	size_t commandBufferSize;

	Command* commands;
	size_t commandCount;

public:
	StreamDriver(Stream& stream)
		:stream(stream)
	{
		commandBufferSize = 0;
		commands = nullptr;
		commandCount = 0;
	}

	void addCommand(char commandChar, CommandHandler handler)
	{
		addCommand(commandChar, handler, nullptr);
	}

	void addCommand(char commandChar, CommandHandler handler, const char* description)
	{
		Command newCommand = { commandChar, handler, description };
		addCommandStruct(newCommand);
	}

	void addCommandStruct(const Command& command)
	{
		commands = realloc(commands, sizeof(*commands) * (commandCount + 1));
		if (commands == nullptr) return;

		commands[commandCount].commandChar = command.commandChar;
		commands[commandCount].handler = command.handler;
		commands[commandCount].description = command.description;
		commandCount += 1;
	}

	void setup()
	{
		addCommand('?', helpHandler, "Print this message");
	}

	void loop()
	{
		checkStream();
	}

private:
	void checkStream()
	{
		while (stream.available() > 0) {
			int c = stream.read();
			if (c == -1) return;

			if (c == '\r' || c == '\n') {
				if (commandBufferSize == 0) break;

				Command* cmd = nullptr;
				for (size_t i = 0; i < commandCount; i++) {
					if (commands[i].commandChar == commandBuffer[0]) {
						cmd = &commands[i];
						break;
					}
				}

				if (cmd != nullptr) {
					cmd->handler(this, commandBuffer + 1, commandBufferSize - 1);
				}

				commandBufferSize = 0;
				break;
			} else {
				if (commandBufferSize >= sizeof(commandBuffer)) return;

				commandBuffer[commandBufferSize++] = (char) c;
			}
		}
	}

	static void helpHandler(StreamDriver* driver, const char* cmd, size_t cmdLen)
	{
		driver->stream.println("Available commands:");
		for (size_t i = 0; i < driver->commandCount; i++) {
			driver->stream.print("  ");
			driver->stream.print(driver->commands[i].commandChar);
			driver->stream.print("  ");
			driver->stream.println(driver->commands[i].description ? driver->commands[i].description : "");
		}
	}
};

#endif

