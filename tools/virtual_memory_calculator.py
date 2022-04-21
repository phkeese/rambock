import math
from os import abort
from typing import List
from sys import argv, path_hooks


class OptionParser:
    def __init__(self, argv: List[str]) -> None:
        self.argv = argv
        self.options = {}

    def _set_option(self, name, value) -> None:
        self.options[name] = value

    def add_flag(self, name: str) -> None:
        flag_string = f"--{name}"
        self.options[name] = flag_string in self.argv
        return self.options[name]

    def add_number(self, name: str) -> None:
        option_string = f"--{name}="
        for arg in self.argv:
            if arg.startswith(option_string):
                value = int(eval(arg.split(option_string)[1]))
                self.options[name] = value
        if name not in self.options:
            self.options[name] = int(eval(input(f"enter {name}:")))

        return self.options[name]

    def get_options(self) -> dict:
        return self.options


class Bunch(object):
    def __init__(self, adict):
        self.__dict__.update(adict)


def error(message: str):
    print(message)
    abort()


def calculate(address_bits: int, page_size: int) -> dict:
    # total address space required
    absolute_bytes_total = 2**address_bits

    # number of pages in the entire space
    page_count = absolute_bytes_total // page_size

    # correct for final number of pages
    bytes_total = page_size * page_count

    # how many bits we need to address each byte within a page
    byte_address_bits = int(math.log2(page_size))

    # how many are left to identify the page itself
    page_index_bits = address_bits - byte_address_bits

    # we need to fit this into the next larger power of 8 bits (1 byte, 2 byte)
    # 0-8   --> 1
    # 9-16  --> 2
    # 17-32 --> 4
    # technically we could use sizes like 24 bits, but let's not for now
    page_index_bytes = math.ceil(2**math.ceil(math.log2(page_index_bits)) / 8)

    # how many page addresses we can fit onto one page (indirection) itself
    page_indices_per_page = math.floor(page_size / page_index_bytes)

    # how many real address bits we gain per indirection
    bits_per_indirection = math.floor(math.log2(page_indices_per_page))

    # how many indirections we need in total to convert a virtual address to a
    # real one
    indirection_count = math.ceil(page_index_bits / bits_per_indirection)

    # we might not end up with a neatly divided address
    # to store the addresses of the first level of indirection, we might not
    # have a complete PTE at the first level
    first_level_bits = page_index_bits % bits_per_indirection
    if first_level_bits == 0:
        first_level_bits = bits_per_indirection
    first_level_pages = 2**first_level_bits

    # when all pages are in use and each level is filled to its maximum capacity
    max_pages_used = 0
    for i in range(indirection_count - 1):
        max_pages_used += pow(page_indices_per_page, i)
    max_pages_used *= first_level_pages
    max_pages_used += 1

    overhead_bytes = max_pages_used * page_size + \
        (absolute_bytes_total - bytes_total)
    overhead_percentage = math.ceil(overhead_bytes / bytes_total * 100)

    output = {
        "address_bits": address_bits,
        "absolute_bytes_total": absolute_bytes_total,
        "bytes_total": bytes_total,
        "page_size": page_size,
        "page_count": page_count,
        "byte_address_bits": byte_address_bits,
        "page_index_bits": page_index_bits,
        "page_index_bytes": page_index_bytes,
        "page_indices_per_page": page_indices_per_page,
        "indirection_count": indirection_count,
        "first_level_pages": first_level_pages,
        "max_pages_used": max_pages_used,
        "overhead_percentage": overhead_percentage,
        "overhead_bytes": overhead_bytes,
        "bits_per_indirection": bits_per_indirection,
    }
    return output


def main():
    parser = OptionParser(argv)

    # total address space required
    address_bits = parser.add_number("address_bits")
    bytes_total = 2**address_bits

    # page size to divide it into
    page_size = parser.add_number("page_size")
    if page_size >= bytes_total:
        error("page size cannot be greater than total memory")

    result = Bunch(calculate(address_bits, page_size))

    print(f"Address size:      {result.address_bits} bits")
    print(f"Total bytes:       {result.bytes_total}")
    print(f"Page size:         {result.page_size}")
    print(f"Page count:        {result.page_count}")
    print(f"Byte address:      {result.byte_address_bits} bits")
    print(f"Page index:        {result.page_index_bits} bits")
    print(f"Index size:        {result.page_index_bytes} bytes")
    print(f"Per page:          {result.page_indices_per_page} indices")
    print(f"Per indirection:   {result.bits_per_indirection} bits")
    print(f"Indirection count: {result.indirection_count}")
    print(f"First level count: {result.first_level_pages}")
    print(
        f"maximum overhead:  {result.max_pages_used} pages ({result.overhead_bytes} bytes, {result.overhead_percentage}%)")


def optimizeOverhead(total: int):
    pass

# def realize(address: int) -> int:
#     bits = 32
#     real_address = 0x00000000

#     # read first level from page #0
#     # index is a the leftmost part
#     index = (address & BITS(first_level_bits) << (bits - first_level_bits))
#     pte_address = 0 + page_index_bytes * index
#     first = 0x00 # read the index
#     level |= (first << (bits - first_level_bits))
#     bits -= first_level_bits

#     # until we reach the byte address, add each level
#     while bits > byte_address_bits:
#         # read index from current level
#         pt_address = first * 128
#         value = 0x55

#     return 0


if __name__ == "__main__":
    main()
