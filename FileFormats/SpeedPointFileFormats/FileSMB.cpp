#include "FileSMB.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <Abstract\IMaterial.h>
#include <Abstract\IResourcePool.h>

using std::stringstream;

#define INFLATE_VEC3(a) a.x, a.y, a.z

SP_NMSPACE_BEG

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	L o a d e r

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSMBLoader::ParseError(const char* msg) const
{
	CLog::Log(S_DEBUG, "[SMBLoader] Error @line %u: %s", m_LineCounter, msg);

	// Also log to stdout, in case we do not implement a logger
	std::cout << "SMBLoader] Error @line " << m_LineCounter << ": " << msg << std::endl;
}

bool CSMBLoader::IsEOF() const
{
	return m_Stream.eof();
}

char CSMBLoader::ReadChar()
{
	char c;
	m_Stream.read(&c, 1);
	return c;
}

string CSMBLoader::TrimString(const string& i)
{
	string r;
	if (i.empty())
		return r;

	unsigned int lastChar = 0;
	bool charFound = false;
	for (unsigned int j = i.length() - 1; j > 0; --j)
	{
		char c = i[j];
		if (c != ' ' && c != '\t' && c != '\n')
		{
			lastChar = j;
			charFound = true;
			break;
		}
	}

	if (!charFound)
		return r;

	charFound = false;
	for (unsigned int k = 0; k <= lastChar; ++k)
	{
		char c = i[k];
		if (charFound || (c != ' ' && c != '\t' && c != '\n'))
		{
			charFound = true;
			r += c;
		}
	}

	return r;
}

bool CSMBLoader::ParseString(const string& i, string& o)
{
	if (i.size() == 0)
		return true;

	// Determine position of the quotes;
	unsigned int quoteCnt = 0;
	char quoteChar = '\'';
	for (unsigned int j = 0; j < i.length(); ++j)
	{
		char c = i[j];

		if (c != ' ' && c != '\t' && c != '\n')
		{
			// If we already found the second quote char, there must not be a non-space character afterwards
			if (quoteCnt == 2)
				return false;
		}
		else if (quoteCnt == 0)
		{
			// Skip spaces before the first quote character
			continue;
		}

		if ((quoteCnt == 0 && (c == '\'' || c == '"')) || (quoteCnt == 1 && c == quoteChar))
		{
			if (quoteCnt == 0)
				quoteChar = c;

			quoteCnt++;
		}
		else
		{
			o += c;
		}
	}

	if (quoteCnt < 2)
		return false;

	return true;
}

bool CSMBLoader::ParseVec3(const string& i, Vec3f& o)
{
	return false;

	///      !!!!!!
	///     !!!!!!!!
	///     !!!!!!!!
	///     !!!!!!!!
	///     !!!!!!!!
	///     !!!!!!!!
	///     !!!!!!!!
	///     !!!!!!!!
	///     !!!!!!!!
	///     !!!!!!!!
	///     !!!!!!!!
	///      !!!!!!
	///
	///
	///      !!!!!!
	///     !!!!!!!!
	///      !!!!!!



	// TODO:




	// 1. Remove brackets (, )
	// 2. Split with comma as delimiter
	// 3. Use atof to parse the 3 parts
	//    If any of the 3 atof's fails, this function returns false.








}

// Returns:
// 	eSMBDREAD_OK
// 	eSMBREAD_EOF
// 	eSMBREAD_EMPTY
//	eSMBREAD_ENDOFBLOCK if bracket checking is enabled and the search found the end of the block
ESMBReadResult CSMBLoader::ReadUntilNext(char until, string *pOut /*= 0*/, const vector<char>* pSkip /*= 0*/, bool checkBrackets /*= false*/, int startBracketCounter /*= 0*/)
{
	unsigned int char_count = 0;
	int bracketCounter = startBracketCounter;
	while (!IsEOF())
	{
		char c = ReadChar();

		if (c == '\n')
			m_LineCounter++;

		if (checkBrackets)
		{
			if (c == '{') bracketCounter++;
			else if (c == '}') bracketCounter--;

			if (bracketCounter < startBracketCounter)
				return eSMBREAD_ENDOFBLOCK;
		}

		if (c == until)
		{
			if (char_count == 0)
				return eSMBREAD_EMPTY;
			else
				return eSMBREAD_OK;
		}
		else if (IS_VALID_PTR(pOut))
		{
			if (IS_VALID_PTR(pSkip))
			{
				bool skip = false;
				for (auto itChar = pSkip->begin(); itChar != pSkip->end(); ++itChar)
				{
					if (c == *itChar)
					{
						skip = true;
						break;
					}
				}

				if (skip)
					continue;
			}

			*pOut += c;
			char_count++;
		}
	}

	return eSMBREAD_EOF;
}

ESMBReadResult CSMBLoader::ReadUntilNext(const vector<char>& until, char* pFoundChar /*= 0*/, string *pOut /*= 0*/, vector<char>* pSkip /*= 0*/, bool checkBrackets /*=false*/, int startBracketCounter /*= 0*/)
{
	unsigned int char_count = 0;
	int bracketCounter = startBracketCounter;
	while (!IsEOF())
	{
		char c = ReadChar();

		if (c == '\n')
			m_LineCounter++;

		if (checkBrackets)
		{
			if (c == '{') bracketCounter++;
			else if (c == '}') bracketCounter--;

			if (bracketCounter < startBracketCounter)
				return eSMBREAD_ENDOFBLOCK;
		}

		bool isKeyChar = false;
		for (auto itChar = until.begin(); itChar != until.end(); itChar++)
		{
			if (c == *itChar)
			{
				isKeyChar = true;
				break;
			}
		}

		if (isKeyChar)
		{
			if (IS_VALID_PTR(pFoundChar))
				*pFoundChar = c;

			return (char_count == 0 ? eSMBREAD_EMPTY : eSMBREAD_OK);
		}
		else if (IS_VALID_PTR(pOut))
		{
			if (IS_VALID_PTR(pSkip))
			{
				bool skip = false;
				for (auto itSkipChar = pSkip->begin(); itSkipChar != pSkip->end(); itSkipChar++)
				{
					if (c == *itSkipChar)
					{
						skip = true;
						break;
					}
				}

				if (skip)
					continue;
			}

			*pOut += c;
			char_count++;
		}
	}

	return eSMBREAD_EOF;
}

// Summary:
// 	Uses the counter for finding the matching closing bracket
// Returns:
// 	eSMBREAD_OK
// 	eSMBREAD_EOF
ESMBReadResult CSMBLoader::ReadUntilClosingBracket(int counterStart /*= 0*/)
{
	int counter = counterStart;
	while (!IsEOF())
	{
		char c = ReadChar();
		if (c == '{')
			++counter;
		else if (c == '}')
			--counter;
		else
			continue;

		if (counter < counterStart)
			return eSMBREAD_OK;
	}

	return eSMBREAD_EOF;
}

// Returns:
// 	eSMBREAD_OK if the chunk has been read properly
// 	eSMBREAD_SKIPPED if the chunk is corrupted
// 	eSMBREAD_EOF if the end of the file was detected before the read could finish, i.e further chunks are NOT readable
//	eSMBREAD_EMPTY if the end of file was reached and no more chunk parameters where loaded.
ESMBReadResult CSMBLoader::GetNextChunkHeader(vector<string> &chunkHeaderParams)
{
	vector<char> keyChars = { ' ', '{', '\t', '\n' };
	vector<char> skipChars = { '\'', '"' };
	while (!IsEOF())
	{
		string buf;
		char foundChar;
		ESMBReadResult result = ReadUntilNext(keyChars, &foundChar, &buf, &skipChars);
		if (result == eSMBREAD_OK)
		{
			if (foundChar == ' ' || foundChar == '\t' || foundChar == '\n')
				chunkHeaderParams.push_back(buf);
		}
		else if (foundChar == '{')
		{
			return eSMBREAD_OK;
		}
	}

	if (chunkHeaderParams.size() > 0)
		return eSMBREAD_EOF;
	else
		return eSMBREAD_EMPTY;
}






// ESMBReadResult ReadUntilNext(char until, string *pOut = 0, const vector<char>* pSkip = 0, bool checkBrackets = false)


// Returns:
//	eSMBREAD_OK
//	eSMBREAD_EMPTY if there was no more property in this block
//	eSMBREAD_ENDOFBLOCK if the block reached its end before property could be read. Only this chunk is definetly broken.
//	eSMBREAD_ENDOFLINE if the semicolon was reached too early in this property line
//	eSMBREAD_EOF if the file reached its end before property could be read (file is broken, previous successful chunks are not!)
//	eSMBREAD_SKIPPED if the property was broken, but could be skipped
ESMBReadResult CSMBLoader::ReadNextProperty(string& name, string& value)
{
	// Reads:
	//	something = 2.1f;
	//	something_other = 2;
	//	something_another = 'text/blabla.bmp';
	//	something_really_other = "another text";
	//	TODO: something_cool = subchunktype 'subchunkname' { something = 2.0f; test = 'bla bla'; }


	// Read property name
	vector<char> skippedChars = { '\t', ' ', '\n', '\'', '"' };
	vector<char> checkChars = { '=', ';' };
	char foundChar;
	ESMBReadResult result = ReadUntilNext(checkChars, &foundChar, &name, &skippedChars, true, 0);
	if (result == eSMBREAD_EOF || result == eSMBREAD_ENDOFBLOCK || foundChar == ';')
	{
		if (result == eSMBREAD_ENDOFBLOCK && name.length() == 0)
			return eSMBREAD_EMPTY;

		ParseError("Invalid property line!");

		if (foundChar == ';')
			return eSMBREAD_ENDOFLINE;

		return result;
	}
	else if (result == eSMBREAD_EMPTY)
	{
		ParseError("Invalid property name (EMPTY)");

		// Try to skip:
		result = ReadUntilNext(';', 0, 0, true);
		if (result == eSMBREAD_EOF || result == eSMBREAD_ENDOFBLOCK)
			return result;
		else
			return eSMBREAD_SKIPPED;
	}


	// Read property value
	result = ReadUntilNext(';', &value, 0, true);
	if (result == eSMBREAD_EOF || result == eSMBREAD_ENDOFBLOCK)
	{
		ParseError("Could not read property value (EOF or ENDOFBLOCK)");
		return result;
	}
	else if (result == eSMBREAD_EMPTY)
	{
		ParseError("Invalid property value (EMPTY)");

		// Already skipped
		return eSMBREAD_SKIPPED;
	}

	return eSMBREAD_OK;
}



void CSMBLoader::ReadSMBFile(const char* filename, std::vector<SSMBMaterial>& materials)
{
	// Open stream

	m_Stream.open(filename, ifstream::in);
	if (!m_Stream.is_open())
	{
		ParseError("Failed open file!");
		return;
	}

	m_LineCounter = 1;

	ESMBReadResult result;

	while (true)
	{
		// Read until starting { of the block
		vector<string> chunkHeaderParams;
		result = GetNextChunkHeader(chunkHeaderParams);

		if (result == eSMBREAD_EMPTY || result == eSMBREAD_EOF)
		{
			if (result == eSMBREAD_EOF)
				ParseError("Damaged file! (EOF reached too early while reading chunk header)");

			m_Stream.close();
			return; // file parsed.
		}

		if (result == eSMBREAD_SKIPPED)
			continue;

#ifdef _DEBUG
		for (auto itChunkHeaderParam = chunkHeaderParams.begin(); itChunkHeaderParam != chunkHeaderParams.end(); itChunkHeaderParam++)
			std::cout << "'" << *itChunkHeaderParam << "' ";

		std::cout << std::endl;
#endif

		// Determine chunk type		
		std::transform(chunkHeaderParams[0].begin(), chunkHeaderParams[0].end(), chunkHeaderParams[0].begin(), ::tolower);

		ISMBChunk* pChunk = 0;
		if (chunkHeaderParams[0] == "mat")
		{
			pChunk = new CSMBMaterialChunk();
		}
		// else ...

		// If chunk type not detected:
		if (pChunk == 0 || !pChunk->CheckParameters(chunkHeaderParams))
		{
			ParseError("Unknown chunk type");
			if (ReadUntilClosingBracket() == eSMBREAD_EOF)
			{
				ParseError("Incomplete chunk!");
				m_Stream.close();
				return;
			}
			continue;
		}

		pChunk->ApplyHeaderParams(chunkHeaderParams);

		// Read properties		
		while (true)
		{
			string property_name, property_value;
			result = ReadNextProperty(property_name, property_value);

			if (result == eSMBREAD_EMPTY || result == eSMBREAD_ENDOFBLOCK)
				break; // no more properties, reached closing }

			if (result == eSMBREAD_EOF)
			{
				ParseError("Damaged file! (EOF reached too early while reading property)");
				m_Stream.close();
				return;
			}

			if (result == eSMBREAD_OK)
			{
#ifdef _DEBUG
				//std::cout << "- '" << property_name << "': '" << property_value << "'" << std::endl;
#endif

				// Set the property
				std::transform(property_name.begin(), property_name.begin(), property_name.end(), ::tolower);

				if (!pChunk->SetProperty(property_name, property_value))
					ParseError("Invalid property");
			}
		}


		// Add material to materials array
		if (pChunk->GetType() == eSMBCHUNK_MATERIAL)
		{
			CSMBMaterialChunk* pMatChunk = dynamic_cast<CSMBMaterialChunk*>(pChunk);
			if (IS_VALID_PTR(pMatChunk))
				materials.push_back(pMatChunk->GetMaterial());
		}

		delete pChunk;
	}
}












///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	W r i t e r

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSMBWriter::CSMBWriter()
	: m_bAppend(false),
	m_nOpenChunks(0)
{
}

string CSMBWriter::SerializeVec3(float x, float y, float z)
{
	stringstream ss;
	ss << "(" << x << "," << y << "," << z << ")";
	return ss.str();
}

void CSMBWriter::ParseError(const char* msg) const
{

	std::cout << "[SMBWriter] Error: " << msg << std::endl;

	// TODO
	//	- include the current line counter in the output!

}

bool CSMBWriter::Prepare(const char* filename, bool append /*=false*/)
{	
	if (m_bAppend = append)
	{
		// First, load existing contents to avoid duplicate chunks
	}
	m_Stream.open(filename, ofstream::out | (append ? ofstream::app : ofstream::trunc));

	return m_Stream.is_open();
}

void CSMBWriter::Close()
{
	if (m_Stream.is_open())
		m_Stream.close();
}

void CSMBWriter::WriteIndent()
{
	if (!m_Stream.is_open() || m_nOpenChunks == 0)
		return;

	for (unsigned i = 0; i < m_nOpenChunks; ++i)
		m_Stream << "    ";
}

void CSMBWriter::OpenChunk(const string& header)
{
	if (!m_Stream.is_open())
	{
		ParseError("Cannot write chunk header: Stream not open!");
		return;
	}

	if (m_bAppend)
	{
		// TODO: Check if this chunk already exists
	}

	WriteIndent();
	m_Stream << header << " {" << std::endl;
	m_nOpenChunks++;
}

void CSMBWriter::CloseChunk()
{
	if (!m_Stream.is_open())
	{
		ParseError("Cannot close chunk: Stream not open!");
		return;
	}

	if (m_nOpenChunks == 0)
	{
		ParseError("Cannot close chunk: No chunk open!");
		return;
	}

	m_nOpenChunks--;
	WriteIndent();
	m_Stream << "}" << std::endl << std::endl;
}

inline void CSMBWriter::WritePropertyLineIntrnl(const string& name, const string& value)
{
	if (!m_Stream.is_open())
		return;

	WriteIndent();
	m_Stream << name << " = " << value << ";" << std::endl;
}

void CSMBWriter::WritePropertyLine(const string& name, const string& strvalue)
{
	stringstream ss; ss << "'" << strvalue << "'";
	WritePropertyLineIntrnl(name, ss.str());
}

void CSMBWriter::WritePropertyLine(const string& name, unsigned int a)
{
	stringstream ss; ss << a;
	WritePropertyLineIntrnl(name, ss.str());
}

void CSMBWriter::WritePropertyLine(const string& name, float a)
{
	stringstream ss; ss << a;
	WritePropertyLineIntrnl(name, ss.str());
}

void CSMBWriter::WritePropertyLine(const string& name, float x, float y, float z)
{
	stringstream ss; ss << "(" << x << "," << y << "," << z << ")";
	WritePropertyLineIntrnl(name, ss.str());
}

















///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	C h u n k s

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CSMBMaterialChunk::CSMBMaterialChunk()
{
}

bool CSMBMaterialChunk::CheckParameters(const vector<string>& params) const
{
	return (params.size() >= 2);
}

void CSMBMaterialChunk::ApplyHeaderParams(const vector<string>& params)
{
	m_Material.name = params[1];
}

bool CSMBMaterialChunk::SetProperty(const string& name, const string& value)
{
	string map;
	if (name == "texture_map")
	{
		if (!CSMBLoader::ParseString(value, map))
			return false;

		m_Material.textureMap = CSMBLoader::TrimString(map);
	}
	else if (name == "normal_map")
	{
		if (!CSMBLoader::ParseString(value, map))
			return false;

		m_Material.normalMap = CSMBLoader::TrimString(map);
	}
	else if (name == "roughness_map")
	{
		if (!CSMBLoader::ParseString(value, map))
			return false;

		m_Material.roughnessMap = CSMBLoader::TrimString(map);
	}
	else
	{
		return false;
	}


	return true;
}

bool CSMBMaterialChunk::HasProperty(const string& name) const
{



	// TODO

	return false;





}

string CSMBMaterialChunk::GetProperty(const string& name) const
{



	// TODO


	return "";




}

void CSMBMaterialChunk::Write(CSMBWriter* pWriter, const SSMBMaterial& material)
{
	if (!IS_VALID_PTR(pWriter))
		return;

	string chunkHeader = "mat '";
	chunkHeader += material.name;
	chunkHeader += "'";

	pWriter->OpenChunk(chunkHeader);

	pWriter->WritePropertyLine("texture_map", material.textureMap.c_str());
	pWriter->WritePropertyLine("normal_map", material.normalMap.c_str());
	pWriter->WritePropertyLine("roughness_map", material.roughnessMap.c_str());

	pWriter->WritePropertyLine("roughness", material.roughness);
	pWriter->WritePropertyLine("diffuse", INFLATE_VEC3(material.diffuse));
	pWriter->WritePropertyLine("emissive", INFLATE_VEC3(material.emissive));
	pWriter->WritePropertyLine("illum_model", (unsigned int)material.illumModel);

	pWriter->CloseChunk();
}



SP_NMSPACE_END
