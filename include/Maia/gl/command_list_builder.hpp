#pragma once

#include <Maia/common.hpp>

#include <Maia/hw/hw.hpp>
#include <Maia/hw/ndma.hpp>

#include <Maia/math/vec3.hpp>
#include <Maia/math/vec2.hpp>

#include <vector>
#include <algorithm>

namespace gl::packets {
	struct [[gnu::packed]] set_poly_attr_packet {
		set_poly_attr_packet(): cmd{0x29} {}

		uint32_t cmd;
		struct {
			uint32_t light_enable : 4;
			uint32_t polygon_mode : 2;
			uint32_t render_back : 1;
			uint32_t render_front : 1;
			uint32_t reserved : 3;
			uint32_t depth_write_for_translucent : 1;
			uint32_t far_plane_intersection : 1;
			uint32_t polygons_behind_displ_1dot_depth : 1;
			uint32_t depth_test : 1;
			uint32_t fog_enable : 1;
			uint32_t alpha : 5;
			uint32_t reserved1 : 3;
			uint32_t polygon_id : 6;
			uint32_t reserved2 : 2;
		};
	};
	static_assert(sizeof(set_poly_attr_packet) == 2 * sizeof(uint32_t));

	struct [[gnu::packed]] begin_vtxs_packet {
		begin_vtxs_packet(uint8_t primitive_type): cmd{0x40}, primitive_type{primitive_type} {
			assert(primitive_type < 4);
		}

		uint32_t cmd;
		struct {
			uint32_t primitive_type : 2;
			uint32_t reserved : 30;
		};
	};
	static_assert(sizeof(begin_vtxs_packet) == 2 * sizeof(uint32_t));

	struct [[gnu::packed]] end_vtxs_packet {
		end_vtxs_packet(): cmd{0x41} {}

		uint32_t cmd;
	};
	static_assert(sizeof(end_vtxs_packet) == 1 * sizeof(uint32_t));

	struct [[gnu::packed]] colour_packet {
		colour_packet(uint8_t red, uint8_t green, uint8_t blue): cmd{0x20}, red{(uint32_t)red >> 3}, green{(uint32_t)green >> 3}, blue{(uint32_t)blue >> 3} {
			if (red > 255 || green > 255 || blue > 255)
				printf("gl: Invalid colours [\u001b[31;1m%d \u001b[32;1m%d \u001b[34;1m%d\u001b[37;1m]\n", red, green, blue);
		}

		colour_packet(const vec3<uint8_t>& c) : colour_packet{c.x, c.y, c.z} {}

		uint32_t cmd;
		struct {
			uint32_t red : 5;
			uint32_t green : 5;
			uint32_t blue : 5;
			uint32_t reserved : 17;
		};
	};
	static_assert(sizeof(colour_packet) == 2 * sizeof(uint32_t));

	struct [[gnu::packed]] normal_packet {
		normal_packet(v10 x, v10 y, v10 z): cmd{0x21}, x{(uint32_t)x}, y{(uint32_t)y}, z{(uint32_t)z} {}
		normal_packet(const vec3f n) : normal_packet{floattov10(n.x), floattov10(n.y), floattov10(n.z)} {}

		uint32_t cmd;
		struct {
			uint32_t x : 10;
			uint32_t y : 10;
			uint32_t z : 10;
			uint32_t reserved : 2;
		};
	};
	static_assert(sizeof(normal_packet) == 2 * sizeof(uint32_t));

	struct [[gnu::packed]] texcoord_packet {
		texcoord_packet(t16 s, t16 t): cmd{0x22}, s{(uint32_t)s}, t{(uint32_t)t} {}
		texcoord_packet(const vec2f& u) : texcoord_packet{floattot16(u.x), floattot16(u.y)} {}

		uint32_t cmd;
		struct {
			uint32_t s : 16;
			uint32_t t : 16;
		};
	};
	static_assert(sizeof(texcoord_packet) == 2 * sizeof(uint32_t));

	struct [[gnu::packed]] vtx_16_packet {
		vtx_16_packet(v16 x, v16 y, v16 z): cmd{0x23}, x{(uint16_t)x}, y{(uint16_t)y}, z{(uint16_t)z} {}
		vtx_16_packet(const vec3f& v): vtx_16_packet{floattov16(v.x), floattov16(v.y), floattov16(v.z)} {}

		uint32_t cmd;
		uint16_t x;
		uint16_t y;
		uint16_t z;
		uint16_t reserved;
	};
	static_assert(sizeof(vtx_16_packet) == 3 * sizeof(uint32_t));

	struct [[gnu::packed]] mtx_mode_packet {
		mtx_mode_packet(int mode): cmd{0x10}, mode{(uint32_t)mode} {}

		uint32_t cmd;
		struct
		{
			uint32_t mode : 2;
			uint32_t reserved : 30;
		};
	};
	static_assert(sizeof(mtx_mode_packet) == 2 * sizeof(uint32_t));

	struct [[gnu::packed]] mtx_push_packet {
		mtx_push_packet(): cmd{0x11} {}

		uint32_t cmd;
	};
	static_assert(sizeof(mtx_push_packet) == 1 * sizeof(uint32_t));

	struct [[gnu::packed]] mtx_pop_packet {
		mtx_pop_packet(int n): cmd{0x12} {}

		uint32_t cmd;
		struct
		{
			uint32_t offset : 6;
			uint32_t reserved : 26;
		};
	};
	static_assert(sizeof(mtx_pop_packet) == 2 * sizeof(uint32_t));

	struct [[gnu::packed]] mtx_identity_packet {
		mtx_identity_packet(): cmd{0x15} {}

		uint32_t cmd;
	};
	static_assert(sizeof(mtx_identity_packet) == 1 * sizeof(uint32_t));

	struct [[gnu::packed]] mtx_load_4x4_packet {
		mtx_load_4x4_packet(): cmd{0x16} {}
		mtx_load_4x4_packet(const m4x4 &v): cmd{0x16} {
			memcpy(this->v, v.m, sizeof(uint32_t) * 16);
			static_assert(sizeof(uint32_t) == sizeof(int));
		}

		uint32_t cmd;
		uint32_t v[4][4];
	};
	static_assert(sizeof(mtx_load_4x4_packet) == 17 * sizeof(uint32_t));

	struct [[gnu::packed]] mtx_translation_packet {
		mtx_translation_packet(uint32_t x, uint32_t y, uint32_t z): cmd{0x1C}, x{x}, y{y}, z{z} {}

		uint32_t cmd;
		uint32_t x, y, z;
	};
	static_assert(sizeof(mtx_translation_packet) == 4 * sizeof(uint32_t));

	struct [[gnu::packed]] mtx_scale_packet {
		mtx_scale_packet(uint32_t x, uint32_t y, uint32_t z): cmd{0x1B}, x{x}, y{y}, z{z} {}

		uint32_t cmd;
		uint32_t x, y, z;
	};
	static_assert(sizeof(mtx_scale_packet) == 4 * sizeof(uint32_t));

	struct [[gnu::packed]] mtx_mult_4x4_packet {
		mtx_mult_4x4_packet(): cmd{0x18} {}
		mtx_mult_4x4_packet(const m4x4 &v): cmd{0x18} {
			memcpy(this->v, v.m, sizeof(uint32_t) * 16);
			static_assert(sizeof(uint32_t) == sizeof(int));
		}

		uint32_t cmd;
		uint32_t v[4][4];
	};
	static_assert(sizeof(mtx_mult_4x4_packet) == 17 * sizeof(uint32_t));

	struct [[gnu::packed]] mtx_mult_4x3_packet {
		mtx_mult_4x3_packet(): cmd{0x19} {}
		mtx_mult_4x3_packet(const m4x3 &v): cmd{0x19} {
			memcpy(this->v, v.m, sizeof(uint32_t) * 12);
			static_assert(sizeof(uint32_t) == sizeof(int));
		}

		uint32_t cmd;
		uint32_t v[4][3];
	};
	static_assert(sizeof(mtx_mult_4x3_packet) == 13 * sizeof(uint32_t));

	struct [[gnu::packed]] mtx_mult_3x3_packet {
		mtx_mult_3x3_packet(): cmd{0x1A} {}
		mtx_mult_3x3_packet(const m3x3 &v): cmd{0x1A} {
			memcpy(this->v, v.m, sizeof(uint32_t) * 9);
			static_assert(sizeof(uint32_t) == sizeof(int));
		}

		uint32_t cmd;
		uint32_t v[3][3];
	};
	static_assert(sizeof(mtx_mult_3x3_packet) == 10 * sizeof(uint32_t));

	template <typename T>
	concept Packet = sizeof(T) >= 4 && (sizeof(T) % 4) == 0;
}

namespace gl {
	struct cmdlist {
		cmdlist(): buffer{}, updated{true} { }

		void append(const packets::Packet auto &packet) {
			const auto *data = reinterpret_cast<const uint8_t*>(&packet);

			for (size_t i = 0; i < sizeof(packet); i++)
				buffer.push_back(*data++);

			updated = true;
		}

		void execute() {
			if(buffer.size() == 0)
				return;
			
			// Taking the advice of glCallList here, to do it ourselves and only flush the region if something changed
			// Flush the area that we are going to DMA if it was updated since last execution
			if(updated) {
				DC_FlushRange(buffer.data(), buffer.size());
				updated = false;
			}
			
			auto dma = [this]{
				// Don't start DMAing while anything else is being DMAed because FIFO DMA is touchy as hell, gets fixed by DSi Revised geometry circuit
				if(!hw::features.revised_dma) {
					while((DMA_CR(0) & DMA_BUSY) || (DMA_CR(1) & DMA_BUSY) || (DMA_CR(2) & DMA_BUSY) || (DMA_CR(3) & DMA_BUSY));
				} else {
					while(DMA_CR(0) & DMA_BUSY);
				}

				// Send the packed list asynchronously via DMA to the FIFO
				DMA_SRC(0) = (u32)buffer.data();
				DMA_DEST(0) = 0x4000400;
				DMA_CR(0) = DMA_FIFO | (buffer.size() / 4);
				while(DMA_CR(0) & DMA_BUSY);
			};

			auto ndma = [this]{
				using namespace ndma;

				Transfer transfer{};
				transfer.startup_mode = ndma::hw::NdmaStartupModes::Arm9GeometryFifo;

				transfer.src = (uint32_t)buffer.data();
				transfer.src_inc = ndma::Transfer::IncementMode::Increment;

				transfer.dst = 0x4000400;
				transfer.dst_inc = ndma::Transfer::IncementMode::Fixed;

				transfer.len = buffer.size();

				ndma9.transfer_sync(transfer);
			};

			constexpr bool use_ndma = false; // Currently we cannot use NDMA since no$gba doesn't support Geometry FIFO start mode
			if (hw::features.ndma && use_ndma)
				ndma();
			else
				dma();
		}

		std::vector<uint8_t> buffer;
		bool updated;
	};
} // namespace gl