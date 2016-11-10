//Using SDL, SDL_image, standard IO, and string

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <cstdlib>
#include <time.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <string>
#include <vector>
#include <sstream>

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 576;

//setting dimensions
const int TILE_WIDTH = 32;
const int TILE_HEIGHT = 32;

const int BELT_WIDTH = 40;
const int BELT_HEIGHT = 40;

const int SPLITTER_N_WIDTH = 80;
const int SPLITTER_N_HEIGHT = 35;
const int SPLITTER_S_WIDTH = 82;
const int SPLITTER_S_HEIGHT = 36;
const int SPLITTER_W_WIDTH = 47;
const int SPLITTER_W_HEIGHT = 79;
const int SPLITTER_E_WIDTH = 46;
const int SPLITTER_E_HEIGHT = 81;

const int CURSOR_BOX_HEIGHT = 32;
const int CURSOR_BOX_WIDTH = 32;

//setting size of Sprite sheets
const int NUMBER_OF_SPRITE_SHEETS_GRASS = 16;

const int NUMBER_OF_SPRITE_SHEETS_BELT_WIDTH = 32;
const int NUMBER_OF_SPRITE_SHEETS_BELT_HEIGHT = 12;
const int NUMBER_OF_SPRITE_SHEETS_BELT = 384; // = 12*32

const int NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH = 16;
const int NUMBER_OF_SPRITE_SHEETS_SPLITTER_HEIGHT = 2;

const int MAX_NUMBER_OF_INPUTS = 16;
const int MAX_NUMBER_OF_OUTPUTS = 16;

const int NUMBER_OF_SPRITESHEETS_CURSOR_BOXES_WIDTH = 4;
const int NUMBER_OF_SPRITESHEETS_CURSOR_BOXES_HEIGHT = 1;

//offsets of the sprites of the splitters in pixels
const int SPLITTER_WEST_OFFSET_N = 2;
const int SPLITTER_NORTH_OFFSET_N = 1;

const int SPLITTER_WEST_OFFSET_E = 6;
const int SPLITTER_NORTH_OFFSET_E = 8;

const int SPLITTER_WEST_OFFSET_S = 6;
const int SPLITTER_NORTH_OFFSET_S = 2;

const int SPLITTER_WEST_OFFSET_W = 1;
const int SPLITTER_NORTH_OFFSET_W = 4;

//maximum frames in the animations
const int MAX_FRAMES = 32;

//the number of frames after which the infobox gets shown if the mouse hasnt moved and the hand is empty
const int maxFramesMouseNotMoved = 60;

//the max width in pixels for the textbox
const int MAX_WIDTH_INFO_BOX = 150;

//counters, holding the number of inputs/outputs there are at the moment
int inputCounter;
int outputCounter;

enum LBeltIODirections
{
	TOP,
	RIGHT,
	BOTTOM,
	LEFT
};

enum LBeltDirections
{
	EAST = 0,
	NORTH = 1,
	EAST_TO_NORTH = 8,
	NORTH_TO_EAST = 9,
	WEST_TO_SOUTH = 10,
	SOUTH_TO_EAST = 11
};

enum LItemHold
{
	NOTHING_HOLD,
	BELT_NORTH_HOLD,
	BELT_EAST_HOLD,
	BELT_SOUTH_HOLD,
	BELT_WEST_HOLD,
	SPLITTER_NORTH_HOLD,
	SPLITTER_EAST_HOLD,
	SPLITTER_SOUTH_HOLD,
	SPLITTER_WEST_HOLD,
	UNDERGROUND_NORTH_HOLD,
	UNDERGROUND_EAST_HOLD,
	UNDERGROUND_SOUTH_HOLD,
	UNDERGROUND_WEST_HOLD,
	HOLD_TOTAL
};

enum LCursorBoxColors
{
	YELLOW,
	RED,
	BLUE,
	GREEN,
	COLORS_TOTAL
};

enum LType
{
	BELT,
	SPLITTER,
	TYPES_TOTAL
};

class Object
{
public:
	//TODO: Sort out, which functions can be private/protected
	Object()
		: in(-1),
		  out(-1), 
		  input(false), 
		  output(false), 
		  inputID(-1), 
		  outputID(-1),
		  xPosition(0),
		  yPosition(0),
		  outputObject(nullptr),
		  inputObject(nullptr)
	{
		count++;	//"counting"
	}

	virtual ~Object()
	{
		count--;	//"counting"
	}

	//virtual Functions that change in the derived classes

	//virtual function render
	virtual void render(int currentframe) const = 0;

	//virtual function to tell the Type of the object
	virtual int getType() const = 0;

	//virtual function for second in-/output, depends on the type of the object
	virtual Object* getOutObject2() const = 0;
	virtual Object* getInObject2() const = 0;
	virtual void setOutObject2(Object* outObj) = 0;
	virtual void setInObject2(Object* inObj) = 0;

	//get pointer to one of the objects this Object outputs to (n is like an ID)
	Object* getOutObject() const;

	//same as getOutObject with in
	Object* getInObject() const;

	//sets the in/output object of this object
	void setInObject(Object* inObj);
	void setOutObject(Object* outObj);

	//adds the content of the given object to its own content
	void getInputFrom(Object* inputtingObject);

	//content of this object is now nextTickContent, nextTickContent gets reset
	void updateContent();

	//resets the inputs of this object
	void resetInput();

	//get In direction of the Object
	int getInDirection() const;

	//set Input direction of the Object
	void setInDirection(int indir);

	//get Out direction of the Object
	int getOutDirection() const;

	//set Output direction of the Object
	void setOutDirection(int outdir);

	//returns if this object is an input
	bool isInput() const;

	//set this objects input state
	void setInput(bool inputstate);

	//returns if this object is an output
	bool isOutput() const;

	//set this objects output state
	void setOutput(bool outputstate);

	//get the inputID
	int getInputID() const;

	//get the outputID
	int getOutputID() const;

	//set the inputID
	void setInputID(int ID);

	//set the outputID
	void setOutputID(int ID);

	//get the x position of this Object on the Grid of Tiles, not in Pixels
	int getX() const;

	//get the y position of this Object
	int getY() const;

	//sets the x position of this Object
	void setX(int xpos);

	//sets the y position of this Object
	void setY(int ypos);

	//get Contents of the Object (Note: for a splitter gives back half of it if it has two outputs)
	virtual double getContent(int ID) const;

	//setting the content for a specific ID
	void setContent(double ratio, int ID);

	//updates this object's in direction
	void updateDirection();

	//resets the object's in and out objects
	void resetIOObjects();

	//updates the object's in and out objects
	void updateIOObjects();

	//giving back if there is input from the specified direction
	bool isInputFromTop() const;
	bool isInputFromRight() const;
	bool isInputFromBottom() const;
	bool isInputFromLeft() const;

	//giving back if there is an object to output to in the specified direction
	bool isOutputToTop() const;
	bool isOutputToRight() const;
	bool isOutputToBottom() const;
	bool isOutputToLeft() const;

	//gets this object's input direction
	int getInputDirection() const;

	//returns if this object has any inputs
	bool hasInput() const;

	//returns if this object has any outputs
	bool hasOutput() const;

	//just returns the number of objects existing
	static int getCount();

protected:
	//in and out directions
	//TODO: make something like a direction class
	int in;
	int out;

	//if this object is an input
	bool input;

	//if this object is an output
	bool output;

	//if this object is an input or output this is its ID
	int inputID;
	int outputID;

	//x and y position of this Object
	int xPosition;
	int yPosition;

	//pointers to the input / output object (for splitters there are extra second ones in the derived class)
	Object* outputObject;
	Object* inputObject;

	//the contents of the object
	double content[MAX_NUMBER_OF_INPUTS];
	double nextTickContent[MAX_NUMBER_OF_INPUTS];

private:
	//a static counter counting how many objects exist
	static int count;

	//TODO: maybe make something like a static vector of pointers to all existing objects to be able to reference all of them without using the global vector of all objects on screen
};

//initializing the static variables
int Object::count = 0;

//the vector containing all the objects on the screen
vector<Object*> objects;

void Object::getInputFrom(Object* inputtingObject)
{
	for (auto i = 0; i < MAX_NUMBER_OF_INPUTS; i++)			//adds the contents of the given object to this objects nextTickContents
		this->nextTickContent[i] += inputtingObject->getContent(i);
}

void Object::updateContent()
{
	for (auto i = 0; i < MAX_NUMBER_OF_INPUTS; i++)		//the content is now nextTickContent
		this->content[i] = this->nextTickContent[i];
	for (auto i = 0; i < MAX_NUMBER_OF_INPUTS; i++)		//reset the nextTickContent
		this->nextTickContent[i] = 0.0;
}

void Object::resetInput()
{
	for (auto i = 0; i < MAX_NUMBER_OF_INPUTS; i++)
		this->content[i] = 0.0;
}

int Object::getCount()
{
	return count;
}

int Object::getInputID() const
{
	return this->inputID;
}

int Object::getOutputID() const
{
	return this->outputID;
}

void Object::setInputID(int ID)
{
	this->inputID = ID;
}

void Object::setOutputID(int ID)
{
	this->outputID = ID;
}

bool Object::isInput() const
{
	return this->input;
}

bool Object::isOutput() const
{
	return this->output;
}

void Object::setInput(bool inputstate)
{
	this->input = inputstate;
}

void Object::setOutput(bool outputstate)
{
	this->output = outputstate;
}

void Object::setInDirection(int indir)
{
	this->in = indir;
}

void Object::setOutDirection(int outdir)
{
	this->out = outdir;
}

int Object::getX() const
{
	return this->xPosition;
}

int Object::getY() const
{
	return this->yPosition;
}

void Object::setX(int xpos)
{
	this->xPosition = xpos;
}

void Object::setY(int ypos)
{
	this->yPosition = ypos;
}

Object* Object::getOutObject() const
{
	return this->outputObject;
}

Object* Object::getInObject() const
{
	return this->inputObject;
}

int Object::getInDirection() const
{
	return this->in;
}

int Object::getOutDirection() const
{
	return this->out;
}

double Object::getContent(int ID) const
{
	return this->content[ID];
}

void Object::setContent(double ratio, int ID)
{
	this->content[ID] = ratio;			//should be save to use [] here cause the vector just got resized to not go out of bounds
}

void Object::updateDirection()
{
	this->setInDirection(this->getInputDirection());
}

void Object::setInObject(Object * inObj)
{
	this->inputObject = inObj;
}

void Object::setOutObject(Object * outObj)
{
	this->outputObject = outObj;
}

void Object::resetIOObjects()
{
	this->inputObject = nullptr;		//releases only the pointer, not the object
	this->outputObject = nullptr;		//same
	this->input = false;
	this->output = false;
}

void Object::updateIOObjects()
{
	//first reset the IO objects...
	this->resetIOObjects();

	//Input Object first:
	if (this->hasInput())	//if there is some input...
	{	//search for it
		for (auto i = 0; i < objects.size(); i++)
		{
			switch(this->getInDirection())
			{
			case TOP:
				if (objects[i]->getOutDirection() == BOTTOM)		//if the object has the right direction
				{
					if (objects[i]->getX() == this->getX()			//if their x coordinates match
						&& objects[i]->getY() == this->getY() - 1)	//it the y coordinates are right
						this->setInObject(objects[i]);				//set the input Object to this object

					if (objects[i]->getType() == LType::SPLITTER	//if the object is a splitter (not this)
						&& objects[i]->getX() == this->getX() - 1	//x coordinates are right
						&& objects[i]->getY() == this->getY() - 1)	//y coordinates are right
						this->setInObject(objects[i]);				//set the input Object to this object

					if (this->getType() == LType::SPLITTER			//if this is a splitter it can also get input at its offside
						&& objects[i]->getX() == this->getX() + 1	//x coordinates have to be right
						&& objects[i]->getY() == this->getY() - 1)	//y coordinates have to be right
						this->setInObject2(objects[i]);				//set this object's second input
				}
				break;
			case RIGHT:
				if (objects[i]->getOutDirection() == LEFT)
				{
					if (objects[i]->getX() == this->getX() + 1
						&& objects[i]->getY() == this->getY())
						this->setInObject(objects[i]);
					if (objects[i]->getType() == LType::SPLITTER
						&& objects[i]->getX() == this->getX() + 1
						&& objects[i]->getY() == this->getY() - 1)
						this->setInObject(objects[i]);
					if (this->getType() == LType::SPLITTER
						&& objects[i]->getX() == this->getX() + 1
						&& objects[i]->getY() == this->getY() + 1)
						this->setInObject2(objects[i]);
				}
				break;
			case BOTTOM:
				if (objects[i]->getOutDirection() == TOP)
				{
					if (objects[i]->getX() == this->getX()
						&& objects[i]->getY() == this->getY() + 1)
						this->setInObject(objects[i]);
					if (objects[i]->getType() == LType::SPLITTER
						&& objects[i]->getX() == this->getX() - 1
						&& objects[i]->getY() == this->getY() + 1)
						this->setInObject(objects[i]);
					if (this->getType() == LType::SPLITTER
						&& objects[i]->getX() == this->getX() + 1
						&& objects[i]->getY() == this->getY() + 1)
						this->setInObject2(objects[i]);
				}
				break;
			case LEFT:
				if (objects[i]->getOutDirection() == RIGHT)
				{
					if (objects[i]->getX() == this->getX() - 1
						&& objects[i]->getY() == this->getY())
						this->setInObject(objects[i]);
					if (objects[i]->getType() == LType::SPLITTER
						&& objects[i]->getX() == this->getX() - 1
						&& objects[i]->getY() == this->getY() - 1)
						this->setInObject(objects[i]);
					if (this->getType() == LType::SPLITTER
						&& objects[i]->getX() == this->getX() - 1
						&& objects[i]->getY() == this->getY() + 1)
						this->setInObject2(objects[i]);
				}
				break;
			default:
				break;
			}
		}
		this->setInput(false);			//this is no Input because something is inputting to it
	}
	else
	{
		this->setInObject(nullptr);
		this->setInObject2(nullptr);
		this->setInput(true);			//this is an Input if there is nothing inputting to i
	}
	//same for the pointer to the output object
	if (this->hasOutput())	//if there is some output
	{	//search it
		for (auto i = 0; i < objects.size(); i++)
		{
			switch(this->getOutDirection())
			{
			case TOP:
				if (objects[i]->getInDirection() == BOTTOM)			//if this object has the right direction
				{
					if (objects[i]->getX() == this->getX()			//the same x coordinate
						&& objects[i]->getY() == this->getY() + 1)	//and the right y coordinate
						this->setOutObject(objects[i]);				//set the output object to this object

					if (objects[i]->getType() == LType::SPLITTER	//if the other object is a splitter...
						&& objects[i]->getX() == this->getX() - 1
						&& objects[i]->getY() == this->getY() + 1)
						this->setOutObject(objects[i]);

					if (this->getType() == LType::SPLITTER			//if this is a splitter it can also give output at its offside
						&& objects[i]->getX() == this->getX() + 1	//x coordinates have to be right
						&& objects[i]->getY() == this->getY() + 1)	//y coordinates have to be right
						this->setOutObject2(objects[i]);			//set this object's second output
				}
				break;
			case RIGHT:
				if (objects[i]->getInDirection() == LEFT)
				{
					if (objects[i]->getX() == this->getX() - 1
						&& objects[i]->getY() == this->getY())
						this->setOutObject(objects[i]);
					if (objects[i]->getType() == LType::SPLITTER
						&& objects[i]->getX() == this->getX() - 1
						&& objects[i]->getY() == this->getY() - 1)
						this->setOutObject(objects[i]);
					if (this->getType() == LType::SPLITTER
						&& objects[i]->getX() == this->getX() - 1
						&& objects[i]->getY() == this->getY() + 1)
						this->setOutObject2(objects[i]);
				}

				break;
			case BOTTOM:
				if (objects[i]->getInDirection() == TOP)
				{
					if (objects[i]->getX() == this->getX()
						&& objects[i]->getY() == this->getY() - 1)
						this->setOutObject(objects[i]);
					if (objects[i]->getType() == LType::SPLITTER
						&& objects[i]->getX() == this->getX() - 1
						&& objects[i]->getY() == this->getY() - 1)
						this->setOutObject(objects[i]);
					if (this->getType() == LType::SPLITTER
						&& objects[i]->getX() == this->getX() + 1
						&& objects[i]->getY() == this->getY() - 1)
						this->setOutObject2(objects[i]);
				}
				break;
			case LEFT:
				if (objects[i]->getInDirection() == RIGHT)
				{
					if (objects[i]->getX() == this->getX() + 1
						&& objects[i]->getY() == this->getY())
						this->setOutObject(objects[i]);
					if (objects[i]->getType() == LType::SPLITTER
						&& objects[i]->getX() == this->getX() + 1
						&& objects[i]->getY() == this->getY() - 1)
						this->setOutObject(objects[i]);
					if (this->getType() == LType::SPLITTER
						&& objects[i]->getX() == this->getX() + 1
						&& objects[i]->getY() == this->getY() + 1)
						this->setOutObject2(objects[i]);
				}
				break;
			default:
				break;
			}
		}
		this->setOutput(false);			//this is no Output because its outputting to something
	}
	else
	{
		this->setOutObject(nullptr);
		this->setOutObject2(nullptr);
		this->setOutput(true);			//this is an Output because there is nothing it is outputting to
	}
}

bool Object::isInputFromTop() const //gives back true, if there is coming some input from above for this object
{
	//checking for an Object from above
	for (auto i = 0; i < objects.size(); i++)			
	{
		if (objects[i]->getOutDirection() == BOTTOM) //output direction needs to be BOTTOM in every case
		{
			if (objects[i]->getX() == this->getX()			//if the x position matches
				&& objects[i]->getY() == this->getY() - 1)	//and the y position is right (the object is one above)
				return true;
			if (objects[i]->getType() == LType::SPLITTER	//if the other object is a splitter, input can come from its offside
				&& objects[i]->getX() == this->getX() - 1	//x coordinate has to be right
				&& objects[i]->getY() == this->getY() - 1)	//y coordinate has to be right
				return true;
			if (this->getType() == LType::SPLITTER			//if  is a splitter, input can also come to the offside
				&& objects[i]->getX() == this->getX() + 1	//if x coordinates are right
				&& objects[i]->getY() == this->getY() - 1)	//if y coordinates are right
				return true;
		}
	}
	return false; //if none of the conditions match, there is no input from above
}

//they are all the same, just rotated...

bool Object::isInputFromRight() const
{
	for (auto i = 0; i < objects.size(); i++)
	{
		if (objects[i]->getOutDirection() == LEFT)
		{
			if (objects[i]->getX() == this->getX() + 1
				&& objects[i]->getY() == this->getY())
				return true;
			if (objects[i]->getType() == LType::SPLITTER
				&& objects[i]->getX() == this->getX() + 1
				&& objects[i]->getY() == this->getY() - 1)
				return true;
			if (this->getType() == LType::SPLITTER
				&& objects[i]->getX() == this->getX() + 1
				&& objects[i]->getY() == this->getY() + 1)
				return true;
		}
	}
	return false;
}

bool Object::isInputFromBottom() const
{
	for (auto i = 0; i < objects.size(); i++)
	{
		if (objects[i]->getOutDirection() == TOP)
		{
			if (objects[i]->getX() == this->getX()
				&& objects[i]->getY() == this->getY() + 1)
				return true;
			if (objects[i]->getType() == LType::SPLITTER
				&& objects[i]->getX() == this->getX() - 1
				&& objects[i]->getY() == this->getY() + 1)
				return true;
			if (this->getType() == LType::SPLITTER
				&& objects[i]->getX() == this->getX() + 1
				&& objects[i]->getY() == this->getY() + 1)
				return true;
		}
	}
	return false;
}

bool Object::isInputFromLeft() const
{
	for (auto i = 0; i < objects.size(); i++)
	{
		if (objects[i]->getOutDirection() == RIGHT)
		{
			if (objects[i]->getX() == this->getX() - 1
				&& objects[i]->getY() == this->getY())
				return true;
			if (objects[i]->getType() == LType::SPLITTER
				&& objects[i]->getX() == this->getX() - 1
				&& objects[i]->getY() == this->getY() - 1)
				return true;
			if (this->getType() == LType::SPLITTER
				&& objects[i]->getX() == this->getX() - 1
				&& objects[i]->getY() == this->getY() + 1)
				return true;
		}
	}
	return false;
}

bool Object::isOutputToTop() const //gives back true, if there is an Object above to give an output to
{
	for (auto i = 0; i < objects.size(); i++)
	{
		if (objects[i]->getInDirection() == BOTTOM)	//if the objects input is from the bottom
		{
			if (objects[i]->getX() == this->getX()			//if x positions match
				&& objects[i]->getY() == this->getY() - 1)	//if y positions are right
				return true;								//=> there is an Object to output to above
			if (this->getType() == LType::SPLITTER
				&& objects[i]->getX() == this->getX() + 1
				&& objects[i]->getY() == this->getY() - 1)
				return true;
			if (objects[i]->getType() == LType::SPLITTER	//again, just like with inputs, a splitter can give its output to the offside
				&& objects[i]->getX() == this->getX() - 1
				&& objects[i]->getY() == this->getY() - 1)
				return true;
		}
	}
	return false; //if none of the conditions match, there is nothing to output to above
}

//just rotated versions again

bool Object::isOutputToRight() const
{
	for (auto i = 0; i < objects.size(); i++)
	{
		if (objects[i]->getInDirection() == LEFT)
		{
			if (objects[i]->getX() == this->getX() + 1
				&& objects[i]->getY() == this->getY())
				return true;
			if (this->getType() == LType::SPLITTER
				&& objects[i]->getX() == this->getX() + 1
				&& objects[i]->getY() == this->getY() + 1)
				return true;
			if (objects[i]->getType() == LType::SPLITTER
				&& objects[i]->getX() == this->getX() + 1
				&& objects[i]->getY() == this->getY() - 1)
				return true;
		}
	}
	return false;
}

bool Object::isOutputToBottom() const
{
	for (auto i = 0; i < objects.size(); i++)
	{
		if (objects[i]->getInDirection() == TOP)
		{
			if (objects[i]->getX() == this->getX()
				&& objects[i]->getY() == this->getY() + 1)
				return true;
			if (this->getType() == LType::SPLITTER
				&& objects[i]->getX() == this->getX() + 1
				&& objects[i]->getY() == this->getY() + 1)
				return true;
			if (objects[i]->getType() == LType::SPLITTER
				&& objects[i]->getX() == this->getX() - 1
				&& objects[i]->getY() == this->getY() + 1)
				return true;
		}
	}
	return false;
}

bool Object::isOutputToLeft() const
{
	for (auto i = 0; i < objects.size(); i++)
	{
		if (objects[i]->getInDirection() == RIGHT)
		{
			if (objects[i]->getX() == this->getX() - 1
				&& objects[i]->getY() == this->getY())
				return true;
			if (this->getType() == LType::SPLITTER
				&& objects[i]->getX() == this->getX() - 1
				&& objects[i]->getY() == this->getY() + 1)
				return true;
			if (objects[i]->getType() == LType::SPLITTER
				&& objects[i]->getX() == this->getX() - 1
				&& objects[i]->getY() == this->getY() - 1)
				return true;
		}
	}
	return false;
}

int Object::getInputDirection() const
{
	//these temporarily save, if there is an input from each side
	bool tleft;
	bool tright;
	bool ttop;
	bool tbottom;

	//calculating all of these here so they dont get calculated too often without purpose
	//these functions get a boolean value wether there is some input from the respective side for the position its called to
	tleft = this->isInputFromLeft();
	tright = this->isInputFromRight();
	ttop = this->isInputFromTop();
	tbottom = this->isInputFromBottom();

	switch (this->getOutDirection())
	{
	case TOP: //output is to the top
		if (tbottom || (tright && tleft)) //if there is input from below or input from both left and right
			return BOTTOM; //Input is from the bottom in every case
		if (tright)
			return RIGHT; //Input is from the right, if something came from the left additionally the above return would have been called
		if (tleft)
			return LEFT; //same as right
		return BOTTOM; //if there are no inputs at all its just from the bottom
					   //dont need a break here, it returns before it anyways
	case RIGHT: //pretty much the same as the first one for all other 3 directions, just rotated...
		if (tleft || (ttop && tbottom))
			return LEFT;
		if (ttop)
			return TOP;
		if (tbottom)
			return BOTTOM;
		return LEFT;
	case BOTTOM:
		if (ttop || (tright && tleft))
			return TOP;
		if (tright)
			return RIGHT;
		if (tleft)
			return LEFT;
		return TOP;
	case LEFT:
		if (tright || (ttop && tbottom))
			return RIGHT;
		if (ttop)
			return TOP;
		if (tbottom)
			return BOTTOM;
		return RIGHT;
	default:
		break;
	}
	return -1; //if somehow nothing worked return is negative
}
 
bool Object::hasInput() const //returns for this Object if it has any inputs (used to determine, if a belt is an inputbelt)
{
	switch (this->getInDirection())
	{
	case TOP: //if the input would come from the Top
		if (this->isInputFromTop()) //if there is input from the top
			return true; //has no inputs
		//otherwise (dont need else, it can only get past the "if", if the statement is wrong
		return false; //has inputs
						  //dont need to break, its always returning before it
	case RIGHT: //the same just rotated
		if (this->isInputFromRight())
			return true;
		return false;
	case BOTTOM:
		if (this->isInputFromBottom())
			return true;
		return false;
	case LEFT:
		if (this->isInputFromLeft())
			return true;
		return false;
	default:
		break;
	}
	return false; //return false just in case something went wrong before
}

bool Object::hasOutput() const //returns for the given object if it has anything to output to (used to determine, if a belt is an outputbelt)
{
	switch (this->getOutDirection())
	{
	case TOP:
		if (this->isOutputToTop())
			return true;
		return false;
	case RIGHT:
		if (this->isOutputToRight())
			return true;
		return false;
	case BOTTOM:
		if (this->isOutputToBottom())
			return true;
		return false;
	case LEFT:
		if (this->isOutputToLeft())
			return true;
		return false;
	default:
		break;
	}
	return false; //return false just in case something went wrong
}

class Belt : public Object
{
public:
	//Initializing
	Belt();

	//Deallocating
	~Belt();

	//renders the belt at its location
	void render(int currentframe) const override;

	int getType() const override;

	//have to define these so I can create Belts, always returning nullptr here
	Object* getInObject2() const override;
	Object* getOutObject2() const override;
	void setInObject2(Object* inObj) override;
	void setOutObject2(Object* outObj) override;
};

class Splitter : public Object
{
public:
	//Initializing
	Splitter();

	//Deallocating
	~Splitter();

	//renders the splitter at its location
	void render(int currentframe) const override;

	int getType() const override;

	double getContent(int ID) const override;

	Object* getInObject2() const override;
	Object* getOutObject2() const override;
	void setInObject2(Object* inObj) override;
	void setOutObject2(Object* outObj) override;

protected:

	//output and input Objects 2 because Splitter has 2 in/outputs
	Object* outputObject2;
	Object* inputObject2;

};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Loads individual image as texture
SDL_Texture* loadTexture(std::string path);

//Create Background image from grass1.png
void initBackground();

//updating the directions of all Belts on Screen
void updateBelts();

//updating the input IDs of all the objects
void updateInIDs();

//The window
SDL_Window* gWindow = nullptr;

//The window renderer
SDL_Renderer* gRenderer = nullptr;

//globally used Font
TTF_Font* gFont = nullptr;

//Cursor box sprite loading
SDL_Rect gCursorBoxSpriteClips[ NUMBER_OF_SPRITESHEETS_CURSOR_BOXES_WIDTH ][ NUMBER_OF_SPRITESHEETS_CURSOR_BOXES_HEIGHT ];
SDL_Texture* gCursorBoxSprite;

//Background grass sprite loading
SDL_Rect gBackgroundSpriteClips[ NUMBER_OF_SPRITE_SHEETS_GRASS ];
SDL_Texture* gBackgroundSprite;

//Belt sprite loading
SDL_Rect gBeltSpriteClips[ NUMBER_OF_SPRITE_SHEETS_BELT_WIDTH ][ NUMBER_OF_SPRITE_SHEETS_BELT_HEIGHT ];
SDL_Texture* gBeltSprite;

//Splitter sprite loading
SDL_Rect gSplitterClipsN[ MAX_FRAMES ];
SDL_Rect gSplitterClipsS[ MAX_FRAMES ];
SDL_Rect gSplitterClipsW[ MAX_FRAMES ];
SDL_Rect gSplitterClipsE[ MAX_FRAMES ];
SDL_Texture* gSplitterSpriteN; //north
SDL_Texture* gSplitterSpriteE; //east
SDL_Texture* gSplitterSpriteS; //south
SDL_Texture* gSplitterSpriteW; //west

//Texture on which the generated Background from initBackground is saved
SDL_Texture* gBackground;

Belt::Belt()
{
}

Belt::~Belt()
{
	//nothing to destroy
}

void Belt::render(int currentframe) const
{
	auto beltDir = 0;
	auto flip = SDL_FLIP_NONE;

	//finding out, what belt type (directions) it is
	switch (this->in)
	{
	case TOP:
		switch (this->out)
		{
		case BOTTOM:
			beltDir = NORTH;
			flip = SDL_FLIP_VERTICAL;
			break;
		case RIGHT:
			beltDir = NORTH_TO_EAST;
			flip = SDL_FLIP_NONE;
			break;
		case LEFT:
			beltDir = NORTH_TO_EAST;
			flip = SDL_FLIP_HORIZONTAL;
			break;
		default:
			break;
		}
		break;

	case RIGHT:
		switch (this->out)
		{
		case BOTTOM:
			beltDir = EAST_TO_NORTH;
			flip = SDL_FLIP_VERTICAL;
			break;
		case TOP:
			beltDir = EAST_TO_NORTH;
			flip = SDL_FLIP_NONE;
			break;
		case LEFT:
			beltDir = EAST;
			flip = SDL_FLIP_HORIZONTAL;
			break;
		default:
			break;
		}
		break;

	case BOTTOM:
		switch (this->out)
		{
		case RIGHT:
			beltDir = SOUTH_TO_EAST;
			flip = SDL_FLIP_NONE;
			break;
		case TOP:
			beltDir = NORTH;
			flip = SDL_FLIP_NONE;
			break;
		case LEFT:
			beltDir = SOUTH_TO_EAST;
			flip = SDL_FLIP_HORIZONTAL;
			break;
		default:
			break;
		}
		break;

	case LEFT:
		switch (this->out)
		{
		case BOTTOM:
			beltDir = WEST_TO_SOUTH;
			flip = SDL_FLIP_NONE;
			break;
		case TOP:
			beltDir = WEST_TO_SOUTH;
			flip = SDL_FLIP_VERTICAL;
			break;
		case RIGHT:
			beltDir = EAST;
			flip = SDL_FLIP_NONE;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	SDL_Rect destinationRect;
	destinationRect.x = this->getX() * TILE_WIDTH - (BELT_WIDTH - TILE_WIDTH) / 2;
	destinationRect.y = this->getY() * TILE_HEIGHT - (BELT_HEIGHT - TILE_HEIGHT) / 2;
	destinationRect.w = BELT_WIDTH;
	destinationRect.h = BELT_HEIGHT;

	SDL_RenderCopyEx(gRenderer, gBeltSprite, &gBeltSpriteClips[currentframe][beltDir], &destinationRect, 0.0, nullptr, flip);

	SDL_Rect cursorBoxDest;
	cursorBoxDest.x = this->getX() * TILE_WIDTH;
	cursorBoxDest.y = this->getY() * TILE_HEIGHT;
	cursorBoxDest.w = CURSOR_BOX_WIDTH;
	cursorBoxDest.h = CURSOR_BOX_HEIGHT;

	if (this->isInput())
		SDL_RenderCopy(gRenderer, gCursorBoxSprite, &gCursorBoxSpriteClips[GREEN][0], &cursorBoxDest);
	if (this->isOutput())
		SDL_RenderCopy(gRenderer, gCursorBoxSprite, &gCursorBoxSpriteClips[BLUE][0], &cursorBoxDest);
}

int Belt::getType() const
{
	return LType::BELT;
}

Object * Belt::getInObject2() const
{
	return nullptr;
}

Object * Belt::getOutObject2() const
{
	return nullptr;
}

void Belt::setInObject2(Object * inObj) {}	//nothing to do here, it has no second Input

void Belt::setOutObject2(Object * outObj) {}	//nothing to do here, it has no second output

Splitter::Splitter()
{
	outputObject2 = nullptr;
	inputObject2 = nullptr;
}

Splitter::~Splitter()
{
	//nothing to destroy
}

void Splitter::render(int cframe) const
{
	SDL_Rect destinationRect;
	SDL_Rect destRectBelt;

	destRectBelt.x = this->getX() * TILE_WIDTH - (BELT_WIDTH - TILE_WIDTH) / 2;
	destRectBelt.y = this->getY() * TILE_HEIGHT - (BELT_HEIGHT - TILE_HEIGHT) / 2;
	destRectBelt.w = BELT_WIDTH;
	destRectBelt.h = BELT_HEIGHT;

	switch (this->getOutDirection())
	{
	case TOP:
		destinationRect.x = this->getX() * TILE_WIDTH - SPLITTER_WEST_OFFSET_N;
		destinationRect.y = this->getY() * TILE_HEIGHT - SPLITTER_NORTH_OFFSET_N;
		destinationRect.w = SPLITTER_N_WIDTH;
		destinationRect.h = SPLITTER_N_HEIGHT;

		SDL_RenderCopy(gRenderer, gBeltSprite, &gBeltSpriteClips[cframe][NORTH], &destRectBelt); //render one belt under the splitter

		destRectBelt.x += TILE_WIDTH; //change x position for the second belt

		SDL_RenderCopy(gRenderer, gBeltSprite, &gBeltSpriteClips[cframe][NORTH], &destRectBelt); //render second belt under the splitter

		SDL_RenderCopy(gRenderer, gSplitterSpriteN, &gSplitterClipsN[cframe], &destinationRect); //render the splitter

		break;

	case BOTTOM:
		destinationRect.x = this->getX() * TILE_WIDTH - SPLITTER_WEST_OFFSET_S;
		destinationRect.y = this->getY() * TILE_HEIGHT - SPLITTER_NORTH_OFFSET_S;
		destinationRect.w = SPLITTER_S_WIDTH;
		destinationRect.h = SPLITTER_S_HEIGHT;

		SDL_RenderCopyEx(gRenderer, gBeltSprite, &gBeltSpriteClips[cframe][NORTH], &destRectBelt, 0.0, nullptr, SDL_FLIP_VERTICAL); //render one belt under the splitter

		destRectBelt.x += TILE_WIDTH; //change x position for the second belt

		SDL_RenderCopyEx(gRenderer, gBeltSprite, &gBeltSpriteClips[cframe][NORTH], &destRectBelt, 0.0, nullptr, SDL_FLIP_VERTICAL); //render second belt under the splitter

		SDL_RenderCopy(gRenderer, gSplitterSpriteS, &gSplitterClipsS[cframe], &destinationRect); //render the splitter

		break;

	case LEFT:
		destinationRect.x = this->getX() * TILE_WIDTH - SPLITTER_WEST_OFFSET_W;
		destinationRect.y = this->getY() * TILE_HEIGHT - SPLITTER_NORTH_OFFSET_W;
		destinationRect.w = SPLITTER_W_WIDTH;
		destinationRect.h = SPLITTER_W_HEIGHT;

		SDL_RenderCopyEx(gRenderer, gBeltSprite, &gBeltSpriteClips[cframe][EAST], &destRectBelt, 0.0, nullptr, SDL_FLIP_HORIZONTAL); //render one belt under the splitter

		destRectBelt.y += TILE_HEIGHT; //change x position for the second belt

		SDL_RenderCopyEx(gRenderer, gBeltSprite, &gBeltSpriteClips[cframe][EAST], &destRectBelt, 0.0, nullptr, SDL_FLIP_HORIZONTAL); //render second belt under the splitter

		SDL_RenderCopy(gRenderer, gSplitterSpriteW, &gSplitterClipsW[cframe], &destinationRect);
		break;
	case RIGHT:
		destinationRect.x = this->getX() * TILE_WIDTH - SPLITTER_WEST_OFFSET_E;
		destinationRect.y = this->getY() * TILE_HEIGHT - SPLITTER_NORTH_OFFSET_E;
		destinationRect.w = SPLITTER_E_WIDTH;
		destinationRect.h = SPLITTER_E_HEIGHT;

		SDL_RenderCopy(gRenderer, gBeltSprite, &gBeltSpriteClips[cframe][EAST], &destRectBelt); //render one belt under the splitter

		destRectBelt.y += TILE_HEIGHT; //change x position for the second belt

		SDL_RenderCopy(gRenderer, gBeltSprite, &gBeltSpriteClips[cframe][EAST], &destRectBelt); //render second belt under the splitter

		SDL_RenderCopy(gRenderer, gSplitterSpriteE, &gSplitterClipsE[cframe], &destinationRect);
		break;
	default:
		break;
	}
}

int Splitter::getType() const
{
	return LType::SPLITTER;
}

double Splitter::getContent(int ID) const
{
	if (this->outputObject != nullptr && this->outputObject2 != nullptr)
		return this->content[ID] / 2;											//if it has 2 outputs it outputs half to each output
	
	return this->content[ID];													//Otherwise just return the normal content
}

Object * Splitter::getInObject2() const
{
	return this->inputObject2;
}

Object * Splitter::getOutObject2() const
{
	return this->outputObject2;
}

void Splitter::setInObject2(Object * inObj)
{
	this->inputObject2 = inObj;
}

void Splitter::setOutObject2(Object * outObj)
{
	this->outputObject2 = outObj;
}

void initBackground()
{
	SDL_Rect dest;

	//Create Texture of Background with SDL_TEXTUREACCESS_TARGET set, so it can be rendered on
	gBackground = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);

	//Define all the Rects for the BackgroundSprite
	for (auto i = 0; i < NUMBER_OF_SPRITE_SHEETS_GRASS; i++)
	{
		gBackgroundSpriteClips[i].x = TILE_WIDTH * i;
		gBackgroundSpriteClips[i].y = 0;
		gBackgroundSpriteClips[i].w = TILE_WIDTH;
		gBackgroundSpriteClips[i].h = TILE_HEIGHT;
	}

	//Set Render Target to the Background
	SDL_SetRenderTarget(gRenderer, gBackground);
	SDL_RenderClear(gRenderer);

	//These do not change so they can be defined here
	dest.w = TILE_WIDTH;
	dest.h = TILE_HEIGHT;

	for (auto i = 0; i < (SCREEN_WIDTH / TILE_WIDTH); i++)
	{
		for (auto j = 0; j < (SCREEN_HEIGHT / TILE_HEIGHT); j++)
		{
			//define the destination x and y
			dest.x = i * TILE_WIDTH;
			dest.y = j * TILE_HEIGHT;

			//Render a random Tile from gBackgroundSprite to the Background
			SDL_RenderCopy(gRenderer, gBackgroundSprite, &gBackgroundSpriteClips[(rand() % NUMBER_OF_SPRITE_SHEETS_GRASS)], &dest);
		}
	}

	//Set the render target to default again
	SDL_SetRenderTarget(gRenderer, nullptr);
}

void initBelts()
{
	//defining all the Clips for belts
	for (auto i = 0; i < NUMBER_OF_SPRITE_SHEETS_BELT_WIDTH; i++)
	{
		for (auto j = 0; j < NUMBER_OF_SPRITE_SHEETS_BELT_HEIGHT; j++)
		{
			gBeltSpriteClips[i][j].x = i * BELT_WIDTH;
			gBeltSpriteClips[i][j].y = j * BELT_HEIGHT;
			gBeltSpriteClips[i][j].w = BELT_WIDTH;
			gBeltSpriteClips[i][j].h = BELT_HEIGHT;
		}
	}

	//defining all the clips for splitters
	for (auto i = 0; i < NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH; i++)
	{
		for (auto j = 0; j < NUMBER_OF_SPRITE_SHEETS_SPLITTER_HEIGHT; j++)
		{
			//for the west splitters
			gSplitterClipsW[i + NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH * j].x = i * SPLITTER_W_WIDTH;
			gSplitterClipsW[i + NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH * j].y = j * SPLITTER_W_HEIGHT;
			gSplitterClipsW[i + NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH * j].w = SPLITTER_W_WIDTH;
			gSplitterClipsW[i + NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH * j].h = SPLITTER_W_HEIGHT;

			//for the east splitters (because they are for some weird reason smaller than the west facing ones)
			gSplitterClipsE[i + NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH * j].x = i * SPLITTER_E_WIDTH;
			gSplitterClipsE[i + NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH * j].y = j * SPLITTER_E_HEIGHT;
			gSplitterClipsE[i + NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH * j].w = SPLITTER_E_WIDTH;
			gSplitterClipsE[i + NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH * j].h = SPLITTER_E_HEIGHT;

			//for the north splitters
			gSplitterClipsN[i + NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH * j].x = i * SPLITTER_N_WIDTH;
			gSplitterClipsN[i + NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH * j].y = j * SPLITTER_N_HEIGHT;
			gSplitterClipsN[i + NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH * j].w = SPLITTER_N_WIDTH;
			gSplitterClipsN[i + NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH * j].h = SPLITTER_N_HEIGHT;

			//for the north splitters (because they are for some weird reason bigger than the north ones)
			gSplitterClipsS[i + NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH * j].x = i * SPLITTER_S_WIDTH;
			gSplitterClipsS[i + NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH * j].y = j * SPLITTER_S_HEIGHT;
			gSplitterClipsS[i + NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH * j].w = SPLITTER_S_WIDTH;
			gSplitterClipsS[i + NUMBER_OF_SPRITE_SHEETS_SPLITTER_WIDTH * j].h = SPLITTER_S_HEIGHT;
		}
	}
}

void initCursorBox()
{
	for (auto i = 0; i < NUMBER_OF_SPRITESHEETS_CURSOR_BOXES_WIDTH; i++)
	{
		for (auto j = 0; j < NUMBER_OF_SPRITESHEETS_CURSOR_BOXES_HEIGHT; j++)
		{
			gCursorBoxSpriteClips[i][j].x = i * CURSOR_BOX_WIDTH;
			gCursorBoxSpriteClips[i][j].y = j * CURSOR_BOX_HEIGHT;
			gCursorBoxSpriteClips[i][j].w = CURSOR_BOX_WIDTH;
			gCursorBoxSpriteClips[i][j].h = CURSOR_BOX_HEIGHT;
		}
	}
}

SDL_Texture* loadTexture(std::string path)
{
	//The final texture
	SDL_Texture* newTexture = nullptr;

	//Load image at specified path
	auto loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == nullptr)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == nullptr)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

bool init()
{
	//Initialization flag
	auto success = true;

	//Initalize randomness
	srand(time(nullptr));

	//Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("Belt Balancer Creator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
		if (gWindow == nullptr)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == nullptr)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				//Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	auto success = true;

	//Load grass sprite
	gBackgroundSprite = loadTexture("graphics/grass1.png");

	initBackground();

	//Load belt sprite
	gBeltSprite = loadTexture("graphics/express-transport-belt.png");

	//Load splitter sprites
	gSplitterSpriteN = loadTexture("graphics/express-splitter-north.png");
	gSplitterSpriteE = loadTexture("graphics/express-splitter-east.png");
	gSplitterSpriteS = loadTexture("graphics/express-splitter-south.png");
	gSplitterSpriteW = loadTexture("graphics/express-splitter-west.png");

	initBelts();

	//Load Cursor box sprite
	gCursorBoxSprite = loadTexture("graphics/cursor-boxes-32x32.png");

	initCursorBox();

	//Open the Font
	gFont = TTF_OpenFont("font/Consolas.ttf", 16);
	if (gFont == nullptr)
	{
		printf("Failed to load Consolas Font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded images
	SDL_DestroyTexture(gBackground);
	SDL_DestroyTexture(gBackgroundSprite);
	SDL_DestroyTexture(gBeltSprite);

	SDL_DestroyTexture(gSplitterSpriteN);
	SDL_DestroyTexture(gSplitterSpriteE);
	SDL_DestroyTexture(gSplitterSpriteS);
	SDL_DestroyTexture(gSplitterSpriteW);

	SDL_DestroyTexture(gCursorBoxSprite);

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

Object* searchObjectAtPos(int x, int y)		//searches an object at the given position and returns a pointer to it
{
	if (!objects.empty())
	{
		for (auto i = 0; i < objects.size(); i++)
		{
			if (objects[i]->getX() == x && objects[i]->getY() == y)	//if the position matches
			{
				return objects[i];									//return the pointer
			}
		}
	}
	return nullptr;							//if nothing is found return the nullptr
}

void hoverTextBox(std::string text, int x, int y) //shows text in a fitting box as overlay on the specified position
{
	//get a Surface of the text
	auto textSurface = TTF_RenderText_Blended_Wrapped(gFont, text.c_str(), {0, 0, 0}, MAX_WIDTH_INFO_BOX); //{0, 0, 0} means black 
	if (textSurface == nullptr)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels 
		auto mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == nullptr)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions 
			auto textWidth = textSurface->w;
			auto textHeight = textSurface->h;

			//Everything successfully loaded
			SDL_Rect hoverRect; //define Rect for drawing the box and setting dimensions
			if (x + textWidth + 20 <= SCREEN_WIDTH) //if this setting for x would not exceed the Screen limitation
				hoverRect.x = x;
			else
				hoverRect.x = SCREEN_WIDTH - textWidth - 20;
			if (y + textHeight + 20 <= SCREEN_HEIGHT) //same for y
				hoverRect.y = y;
			else
				hoverRect.y = SCREEN_HEIGHT - textHeight - 20;

			hoverRect.w = textWidth + 20;
			hoverRect.h = textHeight + 20;

			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF); //White
			SDL_RenderFillRect(gRenderer, &hoverRect); //draw the box

			SDL_Rect textRectdest; //define target Rect for the Text
			textRectdest.x = hoverRect.x + 10; //space to the edge
			textRectdest.y = hoverRect.y + 10;
			textRectdest.w = textWidth;
			textRectdest.h = textHeight;

			SDL_Rect textRectsrc; //define target Rect for the Text
			textRectsrc.x = 0; //space to the edge
			textRectsrc.y = 0;
			textRectsrc.w = textWidth;
			textRectsrc.h = textHeight;

			SDL_RenderCopy(gRenderer, mTexture, &textRectsrc, &textRectdest); //render the text
		}
		//Get rid of old surface 
		SDL_FreeSurface(textSurface);
	}
}

void resetAllInputs()	//the function that deletes all the former contents of the belts and resets the input and output objects (called from updateBelts())
{
	for (auto i = 0; i < objects.size(); i++)
	{
		objects[i]->resetInput();
		objects[i]->resetIOObjects();
	}
}

//TODO: this should be a static function of Object and use the to be done static vector of all objects
void simulateBelts() //the function that updates all the inputs for every object
{
	//go through all the objects
	for (auto i = 0; i < objects.size(); i++)
	{
		//for every object, add the contents of its inputs to its own nextTickContent
		if (objects[i]->getInObject() != nullptr) 
			objects[i]->getInputFrom(objects[i]->getInObject());
		if (objects[i]->getInObject2() != nullptr)
			objects[i]->getInputFrom(objects[i]->getInObject2());
		if (objects[i]->isInput())
			objects[i]->getInputFrom(objects[i]);		//if its an input itself, it gets the same input next tick
	}

	for (auto i = 0; i < objects.size(); i++)	//update content for every object
		objects[i]->updateContent();
}

void updateBelts()
{
	resetAllInputs();

	for (auto i = 0; i < objects.size(); i++)		//for every object in the list set its inputobject and the inputobject's outputobject
		objects[i]->updateDirection();
	for (auto i = 0; i < objects.size(); i++) 
		objects[i]->updateIOObjects();
}

void updateInIDs()
{
	auto IDcounter = 0;
	for (auto i = 0; i < objects.size(); i++)		//for every object in the list update its input ID
	{
		objects[i]->setInputID(-1);					//everything is no input at first...
		if (objects[i]->isInput())					//if an object is an input
		{
			objects[i]->setInputID(IDcounter);		//set its inputID
			objects[i]->setContent(1.0, IDcounter);	//set its content to 1 on its inputID
			IDcounter++;
		}
	}
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Set up the transparancy for the hold item
			SDL_SetTextureBlendMode(gBeltSprite, SDL_BLENDMODE_BLEND);

			//Main loop flag
			auto quit = false;

			//currentTick of animations
			auto currentTick = 0;
			SDL_Rect currentItemHoldRect;

			//counts how long the mouse hasnt moved (for rendering the infobox)
			auto mouseNotMoved = 0;

			//x and y of mouse
			auto x = 0;
			auto y = 0;

			//to determine change of the mouseposition
			int lastx;
			int lasty;

			//for better understanding with the array variables
			auto xArray = 0;
			auto yArray = 0;

			//variable that stores, which item is currently being hold by the player/creator
			int itemHold = NOTHING_HOLD;

			//rect of current hold item
			SDL_Rect currentRect;

			//Event handler
			SDL_Event e;

			//While application is running
			while (!quit)
			{
				//MAIN LOOP

				SDL_GetMouseState(&x, &y); // writes the position of the mouse to x and y

				lastx = xArray; //save x and y before updating them
				lasty = yArray;
				//Used in the Mouseevents
				xArray = (x - (x % TILE_WIDTH)) / TILE_WIDTH;
				yArray = (y - (y % TILE_HEIGHT)) / TILE_HEIGHT;

				if (lastx == xArray && lasty == yArray) //if the mouse hasnt moved out of the tile
				{
					mouseNotMoved++; //increment the counter
				}
				else
				{
					mouseNotMoved = 0; //else reset it
				}

				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					else if (e.type == SDL_KEYDOWN)
					{
						//switch through the keys (where the keys are on normal keyboards, no matter what layout is installed)
						switch (e.key.keysym.scancode)
						{
						case SDL_SCANCODE_1:
							itemHold = BELT_NORTH_HOLD;

							//set current item rect to the correct rect of BeltSpriteClips
							currentItemHoldRect = gBeltSpriteClips[currentTick][NORTH];
							break;

						case SDL_SCANCODE_2:
							itemHold = SPLITTER_NORTH_HOLD;

							//set current item rect to the correct rect of BeltSpriteClips
							currentItemHoldRect = gSplitterClipsN[currentTick];
							break;
						case SDL_SCANCODE_R:
							if (itemHold == BELT_NORTH_HOLD || itemHold == BELT_SOUTH_HOLD || itemHold == BELT_WEST_HOLD || itemHold == BELT_EAST_HOLD)
							{
								//if belt pointing east/north/west (not south)
								if (itemHold < BELT_WEST_HOLD && itemHold >= BELT_NORTH_HOLD) itemHold++;
								else if (itemHold == BELT_WEST_HOLD) itemHold = BELT_NORTH_HOLD;
							}
							else if (itemHold == SPLITTER_NORTH_HOLD || itemHold == SPLITTER_SOUTH_HOLD || itemHold == SPLITTER_WEST_HOLD || itemHold == SPLITTER_EAST_HOLD)
							{
								//if splitter pointing east/north/west (not south)
								if (itemHold < SPLITTER_WEST_HOLD && itemHold >= SPLITTER_NORTH_HOLD) itemHold++;
								else if (itemHold == SPLITTER_WEST_HOLD) itemHold = SPLITTER_NORTH_HOLD;

								//update rect
								switch (itemHold)
								{
								case SPLITTER_NORTH_HOLD:
									currentItemHoldRect = gSplitterClipsN[currentTick];
									break;
								case SPLITTER_SOUTH_HOLD:
									currentItemHoldRect = gSplitterClipsS[currentTick];
									break;
								case SPLITTER_WEST_HOLD:
									currentItemHoldRect = gSplitterClipsW[currentTick];
									break;
								case SPLITTER_EAST_HOLD:
									currentItemHoldRect = gSplitterClipsE[currentTick];
									break;
								default:
									break;
								}
							}
							break;
						case SDL_SCANCODE_Q:
							//just put the item in hand away
							itemHold = NOTHING_HOLD;
							break;
						default:
							break;
						}
					}
					else if (e.type == SDL_MOUSEBUTTONUP)
					{
						auto* temp1 = searchObjectAtPos(xArray - 1, yArray);
						auto* temp2 = searchObjectAtPos(xArray, yArray - 1);
						auto* temp3 = searchObjectAtPos(xArray + 1, yArray - 1);
						auto* temp4 = searchObjectAtPos(xArray - 1, yArray + 1);

						auto isSplitterBlocking = false;		//bool if a splitter is blocking the placing
						if (temp1 != nullptr) isSplitterBlocking = temp1->getType() == LType::SPLITTER &&
																   (temp1->getOutDirection() == LBeltIODirections::TOP ||
																	temp1->getOutDirection() == LBeltIODirections::BOTTOM);
						if (temp2 != nullptr && !isSplitterBlocking) isSplitterBlocking = temp2->getType() == LType::SPLITTER &&
																   (temp2->getOutDirection() == LBeltIODirections::LEFT ||
																	temp2->getOutDirection() == LBeltIODirections::RIGHT);
						//Look if there is something already there before placing
						if (searchObjectAtPos(xArray, yArray) == nullptr && !isSplitterBlocking)
						{	//nothing here, may place
							// mouseclick finished
							auto* belt = new Belt();			//initializing pointer to a Belt
							auto* splitter = new Splitter();	//initializing pointer to a Splitter
							switch (itemHold)
							{
							case BELT_NORTH_HOLD:
								belt->setOutDirection(TOP);		//setting direction (only out is needed, in is calculated by simulate belts())
								belt->setX(xArray);				//setting x coordinate
								belt->setY(yArray);				//setting y coordinate
								objects.push_back(belt);		//adding the pointer to the vector
								break;
							case BELT_EAST_HOLD:
								belt->setOutDirection(RIGHT);	//setting direction (only out is needed, in is calculated by simulate belts())
								belt->setX(xArray);				//setting x coordinate
								belt->setY(yArray);				//setting y coordinate
								objects.push_back(belt);		//adding the pointer to the vector
								break;
							case BELT_SOUTH_HOLD:
								belt->setOutDirection(BOTTOM);	//setting direction (only out is needed, in is calculated by simulate belts())
								belt->setX(xArray);				//setting x coordinate
								belt->setY(yArray);				//setting y coordinate
								objects.push_back(belt);		//adding the pointer to the vector
								break;
							case BELT_WEST_HOLD:
								belt->setOutDirection(LEFT);	//setting direction (only out is needed, in is calculated by simulate belts())
								belt->setX(xArray);				//setting x coordinate
								belt->setY(yArray);				//setting y coordinate
								objects.push_back(belt);		//adding the pointer to the vector
								break;
							case SPLITTER_NORTH_HOLD:
								if (searchObjectAtPos(xArray + 1, yArray) == nullptr)
								{
									if (temp3 != nullptr) if (temp3->getType() == LType::SPLITTER &&
															  (temp3->getInDirection() == LBeltIODirections::LEFT ||
															   temp3->getInDirection() == LBeltIODirections::RIGHT)) break;		//break, if there is a Splitter blocking this splitter
									splitter->setOutDirection(TOP);		//setting direction (only out is needed, in is calculated by simulate belts())
									splitter->setX(xArray);				//setting x coordinate
									splitter->setY(yArray);				//setting y coordinate
									objects.push_back(splitter);		//adding the pointer to the vector
								}
								break;
							case SPLITTER_EAST_HOLD:
								if (searchObjectAtPos(xArray, yArray + 1) == nullptr)
								{
									if (temp4 != nullptr) if (temp4->getType() == LType::SPLITTER &&
										(temp4->getInDirection() == LBeltIODirections::TOP ||
											temp4->getInDirection() == LBeltIODirections::BOTTOM)) break;		//break, if there is a Splitter blocking this splitter
									splitter->setOutDirection(RIGHT);	//setting direction (only out is needed, in is calculated by simulate belts())
									splitter->setX(xArray);				//setting x coordinate
									splitter->setY(yArray);				//setting y coordinate
									objects.push_back(splitter);		//adding the pointer to the vector
								}
								break;
							case SPLITTER_SOUTH_HOLD:
								if (searchObjectAtPos(xArray + 1, yArray) == nullptr)
								{
									if (temp3 != nullptr) if (temp3->getType() == LType::SPLITTER &&
										(temp3->getInDirection() == LBeltIODirections::LEFT ||
											temp3->getInDirection() == LBeltIODirections::RIGHT)) break;		//break, if there is a Splitter blocking this splitter
									splitter->setOutDirection(BOTTOM);	//setting direction (only out is needed, in is calculated by simulate belts())
									splitter->setX(xArray);				//setting x coordinate
									splitter->setY(yArray);				//setting y coordinate
									objects.push_back(splitter);		//adding the pointer to the vector
								}
								break;
							case SPLITTER_WEST_HOLD:
								if (searchObjectAtPos(xArray, yArray + 1) == nullptr)
								{
									if (temp4 != nullptr) if (temp4->getType() == LType::SPLITTER &&
										(temp4->getInDirection() == LBeltIODirections::TOP ||
											temp4->getInDirection() == LBeltIODirections::BOTTOM)) break;		//break, if there is a Splitter blocking this splitter
									splitter->setOutDirection(LEFT);	//setting direction (only out is needed, in is calculated by simulate belts())
									splitter->setX(xArray);				//setting x coordinate
									splitter->setY(yArray);				//setting y coordinate
									objects.push_back(splitter);		//adding the pointer to the vector
								}
								break;
							case NOTHING_HOLD:
								break;
							default:	//shouldnt actually happen, but just in case
								break;
							}
							updateBelts();		//update the belts
							updateInIDs();		//update the inputIDs and stuff
						}
					}
				}

				//update current tick
				if (currentTick < NUMBER_OF_SPRITE_SHEETS_BELT_WIDTH) currentTick++;
				if (currentTick >= NUMBER_OF_SPRITE_SHEETS_BELT_WIDTH) currentTick = 0;

				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				//Render background texture to screen
				SDL_RenderCopy(gRenderer, gBackground, nullptr, nullptr);

				//Render all objects
				for (auto i = 0; i < objects.size(); i++)
					objects[i]->render(currentTick);

				//update frame of currently held item
				switch (itemHold)
				{
				case BELT_NORTH_HOLD:
				case BELT_SOUTH_HOLD:
					currentItemHoldRect = gBeltSpriteClips[currentTick][NORTH];
					break;
				case BELT_EAST_HOLD:
				case BELT_WEST_HOLD:
					currentItemHoldRect = gBeltSpriteClips[currentTick][EAST];
					break;
				case SPLITTER_NORTH_HOLD:
					currentItemHoldRect = gSplitterClipsN[currentTick];
					break;
				case SPLITTER_SOUTH_HOLD:
					currentItemHoldRect = gSplitterClipsS[currentTick];
					break;
				case SPLITTER_WEST_HOLD:
					currentItemHoldRect = gSplitterClipsW[currentTick];
					break;
				case SPLITTER_EAST_HOLD:
					currentItemHoldRect = gSplitterClipsE[currentTick];
					break;
				default:
					break;
				}

				// if something is being held, update currentRect
				if (itemHold != NOTHING_HOLD)
				{
					switch (itemHold)
					{
					case BELT_NORTH_HOLD:
					case BELT_SOUTH_HOLD:
					case BELT_WEST_HOLD:
					case BELT_EAST_HOLD:
						currentRect.x = x - (x % TILE_WIDTH) - 4;
						currentRect.y = y - (y % TILE_HEIGHT) - 4;
						currentRect.w = BELT_WIDTH;
						currentRect.h = BELT_HEIGHT;
						break;
					case SPLITTER_NORTH_HOLD:
						currentRect.x = x - (x % TILE_WIDTH) - SPLITTER_WEST_OFFSET_N;
						currentRect.y = y - (y % TILE_HEIGHT) - SPLITTER_NORTH_OFFSET_N;
						currentRect.w = SPLITTER_N_WIDTH;
						currentRect.h = SPLITTER_N_HEIGHT;
						break;
					case SPLITTER_SOUTH_HOLD:
						currentRect.x = x - (x % TILE_WIDTH) - SPLITTER_WEST_OFFSET_S;
						currentRect.y = y - (y % TILE_HEIGHT) - SPLITTER_NORTH_OFFSET_S;
						currentRect.w = SPLITTER_S_WIDTH;
						currentRect.h = SPLITTER_S_HEIGHT;
						break;
					case SPLITTER_WEST_HOLD:
						currentRect.x = x - (x % TILE_WIDTH) - SPLITTER_WEST_OFFSET_W;
						currentRect.y = y - (y % TILE_HEIGHT) - SPLITTER_NORTH_OFFSET_W;
						currentRect.w = SPLITTER_W_WIDTH;
						currentRect.h = SPLITTER_W_HEIGHT;
						break;
					case SPLITTER_EAST_HOLD:
						currentRect.x = x - (x % TILE_WIDTH) - SPLITTER_WEST_OFFSET_E;
						currentRect.y = y - (y % TILE_HEIGHT) - SPLITTER_NORTH_OFFSET_E;
						currentRect.w = SPLITTER_E_WIDTH;
						currentRect.h = SPLITTER_E_HEIGHT;
						break;
					default:
						break;
					}
				}

				switch (itemHold)
				{
				case BELT_NORTH_HOLD:
				case BELT_EAST_HOLD:
					//Render current hold Item to screen
					SDL_SetTextureAlphaMod(gBeltSprite, 180);
					SDL_RenderCopy(gRenderer, gBeltSprite, &currentItemHoldRect, &currentRect);
					SDL_SetTextureAlphaMod(gBeltSprite, 255);
					break;
				case BELT_SOUTH_HOLD:
					//Render just like north but mirrored
					SDL_SetTextureAlphaMod(gBeltSprite, 180);
					SDL_RenderCopyEx(gRenderer, gBeltSprite, &currentItemHoldRect, &currentRect, 0.0, nullptr, SDL_FLIP_VERTICAL);
					SDL_SetTextureAlphaMod(gBeltSprite, 255);

					break;
				case BELT_WEST_HOLD:
					//Render just like east but mirrored
					SDL_SetTextureAlphaMod(gBeltSprite, 180);
					SDL_RenderCopyEx(gRenderer, gBeltSprite, &currentItemHoldRect, &currentRect, 0.0, nullptr, SDL_FLIP_HORIZONTAL);
					SDL_SetTextureAlphaMod(gBeltSprite, 255);
					break;
				case SPLITTER_SOUTH_HOLD:
					//Render current held Splitter to screen
					SDL_SetTextureAlphaMod(gSplitterSpriteS, 180);
					SDL_RenderCopy(gRenderer, gSplitterSpriteS, &currentItemHoldRect, &currentRect);
					SDL_SetTextureAlphaMod(gSplitterSpriteS, 255);
					break;
				case SPLITTER_NORTH_HOLD:
					//Render current held Splitter to screen
					SDL_SetTextureAlphaMod(gSplitterSpriteN, 180);
					SDL_RenderCopy(gRenderer, gSplitterSpriteN, &currentItemHoldRect, &currentRect);
					SDL_SetTextureAlphaMod(gSplitterSpriteN, 255);
					break;
				case SPLITTER_WEST_HOLD:
					//Render current held Splitter to screen
					SDL_SetTextureAlphaMod(gSplitterSpriteW, 180);
					SDL_RenderCopy(gRenderer, gSplitterSpriteW, &currentItemHoldRect, &currentRect);
					SDL_SetTextureAlphaMod(gSplitterSpriteW, 255);
					break;
				case SPLITTER_EAST_HOLD:
					//Render current held Splitter to screen
					SDL_SetTextureAlphaMod(gSplitterSpriteE, 180);
					SDL_RenderCopy(gRenderer, gSplitterSpriteE, &currentItemHoldRect, &currentRect);
					SDL_SetTextureAlphaMod(gSplitterSpriteE, 255);
					break;
				default:
					break;
				}

				//show infobox
				//if (false)
				//{
					if (mouseNotMoved >= maxFramesMouseNotMoved)
					{
						//TODO: need to search for the right object inside the vector now... rewrite
						
						auto* hoverObject = searchObjectAtPos(xArray, yArray);
						if (hoverObject != nullptr) //if there is an object the mouse is hovering over
						{
							std::string beltcontent = "";
							for (auto i = 0; i < MAX_NUMBER_OF_INPUTS; i++)
							{
								if (i != 0) beltcontent += "\n";
								std::stringstream convert; //just to convert the numbers to string
								convert << i + 1 << ". input: " << hoverObject->getContent(i);
								beltcontent += convert.str();
							}
							hoverTextBox(beltcontent, x, y);
						}
					}
				//}

				simulateBelts();

				//Update screen
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}

