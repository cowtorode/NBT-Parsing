## :floppy_disk: NBT Parser Showcase!
Minecraft is heavily reliant on **Named Binary Tag** data or **NBT** data for short. NBT data is a binary format which stores data by organizing a list of tags. An NBT tag is a container that includes:
- A **type ID** that describes the kind of data in the container (i.e. TAG_Int, TAG_String, TAG_List)
- A **name** (as a UTF-8 string which is length prefixed),
- And the **payload** (the actual data).

Below is a breakdown of how NBT data is represented in hexadecimal.

```
0300 044e 616d 6500 0000 00

bytes    | description
........ | ........
03       | Type ID.
         | An 8-bit integer representing the data type of the
         | NBT tag, in this case, 03 refers to an NBT_Int type
0004     | Name Length.
         | A 16-bit signed big-endian integer representing the
         | size of the name in bytes
4e616d65 | Name.
         | The data of a string that suffixes it's length in
         | bytes
00000000 | Payload.
         | A big-endian integer value that the tag stores, in
         | this case, the value 0.
```
*Figure 1. NBT tag Binary Representation Breakdown*

Now here is an example with multiple NBT tags encoded within a TAG_Compound, (a special type of tag that stores multiple NBT tags in the NBT tag data field until a TAG_End marker is reached).

```json
"compound":
{
  "Health": 20,
  "Name": "Zombie",
  "Position": [100.5, 64.0, -32.0]
}
```
*Figure 2. TAG_Compound*

In binary, this would be represented as follows in hexadecimal with applicable ASCII translation:

```
0a 0008 compound
  03 0006 Health 20
  08 0004 Name 0006 Zombie
  09 0008 Position 05 00000003 100.5 64.0 -32.0
00
```
*Figure 3. TAG_Compound Binary Representation*

A few things to notice:
- Compound tags end with `0x00` (TAG_End)
- TAG_String's payload contains a 16-bit big-endian length prefix, and the UTF-8 string data
- TAG_List's payload contains an 8-bit type ID of what data is being listed, as well as a 32-bit signed big-endian length value.
## :bookmark: MOSSS NBT Parser
After a couple days of programming, I have a nice prototype to show for how MOSSS will parse NBT data from, for example, Minecraft anvil (.mca) files for reading region file data into memory efficiently.

NBT is unique because each tag maps a key-value relationship, much like a hashmap. Most if not all NBT parsers work by parsing the entire NBT data structure into a hashmap of NBT tags into memory, and then if you want to access data from the NBT structure, you reference this hashmap. This approach is not bad, in fact the parser that I have produced will have full support for this type of architecture. However, I see a much better way of parsing NBT data. Instead of decoding the entire NBT data structure into a hashmap object and then accessing that hashmap for data in order to construct say a Chunk object, I have given MOSSS's NBT Parser the ability to **write directly to user-defined structures,** while NBT data is being parsed using a system called an **NBTRouter**. This offers several advantages to the traditional method of NBT parsing, because it allows for zero-copy object construction, and you don't have to hash twice for each NBT key.
```
      raw NBT                        raw NBT
         |                              |
         v                              v
Traditional NBTParser            MOSSS NBT Parser |-- NBTRouter
         |                              |
         |                              | ✅ Writes directly to
         |                              | chunk according to
         | Maps NBT data to             | NBTRouter configuration
         | hashmap in memory            | using less memory, and
         |                              | avoiding unnecessary
         |                              | copying and hashing
         v                              v
      hashmap                         Chunk
         |
         | ⛔ Copies data from
         | hashmap into chunk
         v
       Chunk
```
*Figure 4. Traditional NBT Parser vs MOSSS NBT Parser*

In order to parse directly to, say a Chunk, you need to configure an NBTRouter with routing methods which redirect data from the NBT data structure directly to the Chunk object as it's read. Then, the following code will fully construct the chunk from the data that's parsed.

```cpp
  NBTRouter router;

  /* router configuration */

  NBTParser parser;

  parser.set_router(router);
  parser.feed(decompressed_chunk, sizeof_decompressed_chunk);

  Chunk* chunk = new Chunk;

  // parses NBT data directly into the Chunk*
  parser.parse(chunk);
```
*Figure 5. Parsing Code*
Note that currently, the NBTRouter has support for all NBT data types.

Now the question becomes, how do you configure the NBTRouter to do this?
## MOSSS NBTRouter
An NBTRouter is designed to map the names of tags to a router function. Router functions are designed to accept the pointer that you initially passed to the `NBTParser::parse(void*)` function, as well as the actual data that you're going to write to that object. An example of a router function is below:

```cpp
void route_xpos(Chunk* chunk, int32_t cx)
{
  // route_xpos is a friend function in Chunk
  chunk->set_cx(cx);
}
```
*Figure 6. Router Function*

Next, you need to assign this router function to an identifier in the NBT file. That way, whenever the NBTParser encounters the NBT tag with the name that you want this router to refer to, (in this case, whenever it encounters a TAG_Int by the name "xPos"), it will call this method to write to the Chunk. The way that you assign a router function to an NBTRouter is shown below:

```cpp
  NBTRouter router;

  /* NBTRouters internally store an std::unordered_map mapping an
     std::string to a const void*, and the functions to write to
     this map enforce security on what this void* can be. In this
     case, an int_router is expected to be:

       void(*)(void*, int32_t)

     and if you'll notice, the route_xpos takes a Chunk*, not a
     void*. This is why it's casted. */
  router.set_int_router("xPos", (int_router) route_xpos);
```
*Figure 7. Router Configuration*

The next question you might have would be, "what happens when you encounter a TAG_Compound when parsing data?" and the answer to that question is that, you simply nest NBTRouters. The NBTParser maintains an NBTRouter stack which stores which NBTRouter the current compound is using. Whenever a compound is terminated using TAG_End, the top router is simply popped off of the stack, and control is returned to the previous NBTRouter. The code to add an NBTRouter to a compound is shown below:

```cpp
  NBTRouter sections_router;

  router.set_compound_router("sections", sections_router);
```
*Figure 8. Router Configuration with Compounds*
This might make writing to your object a little difficult, however. This is because each router function accepts the object that you passed to `NBTParser::parse(void*)`, and if you're parsing well within several TAG_Compounds, you'll be writing to an object that's highly nested within the initial object you passed to the parser, which means you're going to be dereferencing the same data repeatedly for every tag in a highly nested compound which is very slow. For example, if I was writing to a ChunkSection and I was midway through parsing say the block_state_palette property, in order to write to that value in the Chunk object passed to the parse function, I would have to access chunk's ChunkSections at the correct index (as it is an array), and then go to the correct block state, and go to the correct block state palette at the correct index, and append to it. I would have to do all this dereferencing for each palette entry in the TAG_Compound, as it is a list of compounds.

```
 "sections": [{
   "Y": TAG_Byte,
   "block_states":
   {
     "block_state_palette": [{
        Name: TAG_String
        Properties: {
          Name: TAG_String
        }
     }, {...}],
     "data": TAG_Long_Array
   },
   "biomes":
   {
     "block_state_palette": TAG_List TAG_String,
     "data": TAG_Long_Array
   }
   "BlockLight": TAG_Byte_Array,
   "SkyLight": TAG_Byte_Array
 }, {...}]
```
*Figure 9. ChunkSections NBT Structure https://minecraft.wiki/w/Chunk_format*

To fix all of this unnecessary dereferencing, one reference is made and stored along with the NBTRouter in the parser's router stack, so instead of passing the Chunk* around to these routing methods, the new reference made is passed instead, and it's only dereferenced once. This reference comes from another method in the NBTRouter called the `next_handle` method, which is defined as:

```cpp
typedef void*(*handle_mutator)(void*);
```
*Figure 10. handle_mutator*

This is another part of NBTRouter configuration that you must do in order to have this type of behavior, however defaultly, this method will return the pointer that is passed to it, so you don't have to use it if you don't want to.
##:jigsaw: What’s Next?
This parser already provides a fast, flexible foundation for reading NBT into your own data structures with minimal overhead. Some next steps include:
- Adding **benchmark comparisons** against traditional parsers.
- Supporting **writing** NBT data back to binary.
- Improving **debugging tools** for router mismatches or tag type errors.
- Including a default parse method that requires no router configuration, but instead parses traditionally into an NBTCompound object.

So whether for use in MOSSS, or use outside of MOSSS, this NBTParser provides the best of all worlds with performance, control, and traditional NBT parsing in mind. You're no longer limited to building an intermediate tree or hashmap, instead, you can now directly map the binary NBT to your user-defined structures which drastically improves efficiency, especially in data-heavy scenarios like Minecraft region file parsing.

I'm happy to be presenting this to MOSSS, as this is integral to Minecraft's item and world API! If you have any questions or concerns, please message me.

*May 23rd, 2025*
